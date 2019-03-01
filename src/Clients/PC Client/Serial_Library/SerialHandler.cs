using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;

namespace Serial_Library
{

    public static class Comm
    {
        static List<CustomSerial> Ports = null;
        public enum e_MessageType
        {
            Binary,
            String
        }
        public static void Open(string PortName, int BaudRate, int MessageSize)
        {
            CustomSerial port = new CustomSerial(PortName, BaudRate, MessageSize);
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


        public CustomSerial(string PortName, int BaudRate, int MessageSize)
        {

            try
            {
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
            if (this._messageSize>0) //<-- assume fixed length messages are binary
            {
                int byte_index = 0;
                byte[] byte_array = new byte[this._messageSize];

                while (byte_index < this._messageSize && this.Port.BytesToRead>0)
                {
                    byte this_byte = 0;
                    this_byte = (byte)this.Port.ReadByte();
                    byte_array[byte_index++] = this_byte;
                }
                this.DataBuffer.Add(new SerialMessage(Comm.e_MessageType.Binary, byte_array));
            }
            else
            {
                
                this.DataBuffer.Add(new SerialMessage(Comm.e_MessageType.String, Encoder.GetBytes(this.Port.ReadLine())));
            }
            MessageCount++;
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

        public void Drop_Message(int MessageNumber)
        {
            this.DataBuffer.Remove(this.DataBuffer[MessageNumber]);
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
