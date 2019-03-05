using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;

namespace Serial_Library
{

    public static class Comm
    {
        public static List<CustomSerial> Ports = new List<CustomSerial>();
        public enum e_MessageType
        {
            Binary,
            String
        }
        public static void Open(string PortName, int BaudRate, int MessageSize, object Serializer_Type = null)
        {
            CustomSerial port = new CustomSerial(PortName, BaudRate, MessageSize,Serializer_Type);
            Ports.Add(port);
        }
    }

    public class CustomSerial
    {
        public SerialPort Port = null;
        public List<SerialMessage> DataBuffer = new List<SerialMessage>();
        private int _messageSize;
        System.Text.ASCIIEncoding Encoder = new System.Text.ASCIIEncoding();
        public int MessageCount = 0;
        private object _serializer_Type = null;


        public CustomSerial(string PortName, int BaudRate, int MessageSize, object Serializer_Type = null)
        {

            try
            {
                _serializer_Type = Serializer_Type;
                Port = new SerialPort(PortName, BaudRate, Parity.None, 8, StopBits.One);
                _messageSize = MessageSize;
                Port.DataReceived += Port_DataReceived;
                Port.ReadBufferSize = MessageSize;
                Port.WriteBufferSize = MessageSize;
                Port.NewLine = "\r";
                Port.Open();
            }
            catch
            {

            }
        }

        private void Port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            this._getData();
        }

        private void _getData()
        {
            if (this._messageSize > 0) //<-- assume fixed length messages are binary
            {
                int byte_index = 0;
                byte[] byte_array = new byte[this._messageSize];
                int Running_Check_Sum = 0;
                while (byte_index < this._messageSize && this.Port.BytesToRead > 0)
                {
                    byte this_byte = 0;
                    this_byte = (byte)this.Port.ReadByte();
                    byte_array[byte_index++] = this_byte;
                    //If we missed something, we would be in mid stream of the data so stop at a record terminator.
                    if (byte_index > 2 && (byte_array[byte_index - 1] == 255 && byte_array[byte_index - 2] == 255))
                    {
                        //We have added a record terminator to the check sum, remove it. 
                        Running_Check_Sum -= byte_array[byte_index - 1];
                        break;
                    }
                    //Keep a total of the byte values
                    Running_Check_Sum += this_byte;
                }
                //Last 2 bytes of data are the record terminator. The 2 before that are the check sum.
                //read the 16 bit checksum for the record as 2 separated bytes
                int Message_Check_Sum = byte_array[this._messageSize - 4];
                Message_Check_Sum += byte_array[this._messageSize - 3];

                //the checksum value was added to the runnign check sum bytes. When it is subtracted, the value left should equal the checksum.
                bool valid = (Running_Check_Sum - Message_Check_Sum) == Message_Check_Sum;
                //read the terminator
                int Record_Terminator = BitConverter.ToUInt16(byte_array, this._messageSize - 2);
                if (Record_Terminator == 65535 && valid)
                {
                    //if both checks prove out, assume the message is ok, and add it to the message buffer.
                    this.DataBuffer.Add(new SerialMessage(Comm.e_MessageType.Binary, byte_array));
                    MessageCount++;
                }
            }
            else
            {

                this.DataBuffer.Add(new SerialMessage(Comm.e_MessageType.String, Encoder.GetBytes(this.Port.ReadLine())));
            }
            
        }

        public object Get_Object(int MessageNumber)
        {
            if (this.DataBuffer[MessageNumber].Type == Comm.e_MessageType.String)
                return Get_Message_String(MessageNumber);
            else if (this.DataBuffer[MessageNumber].Type == Comm.e_MessageType.Binary)
                return Get_Message_Bytes(MessageNumber);
            else
                return null;
        }

        public string Get_Message_String(int MessageNumber)
        {
            return Encoder.GetString(this.DataBuffer[MessageNumber].Raw);
        }

        public byte[] Get_Message_Bytes(int MessageNumber)
        {
            return this.DataBuffer[MessageNumber].Raw;
        }

        public object Get_Message_Serialized(int MessageNumber)
        {
            
            if (this.MessageCount > 0 )
            {
                if (this._serializer_Type is Message_Serializer.Message_structs.s_spindle)
                    return ((Message_Serializer.Message_structs.s_spindle)this._serializer_Type).FromArray(this.DataBuffer[MessageNumber].Raw);
            }
            return null;
        }

        public void Drop_Message(int MessageNumber)
        {
            this.DataBuffer.Remove(this.DataBuffer[MessageNumber]);
            MessageCount--;
        }

    }
    public class SerialMessage
    {
        public byte[] Raw = null;
        public bool Complete = false;
        public Comm.e_MessageType Type;

        public SerialMessage(Comm.e_MessageType MessageType, byte[] MessageData)
        {
            Raw = MessageData;
            this.Type = MessageType;
        }


    }
}
