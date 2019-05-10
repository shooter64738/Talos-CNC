using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Message_Serializer
{
    public class Message_structs
    {
        public class s_spindle
        {
            public float Rpm;
            public byte Direction;
            public byte State;

            public byte[] ToArray()
            {
                var stream = new System.IO.MemoryStream();
                var writer = new System.IO.BinaryWriter(stream);

                writer.Write(this.Rpm);
                writer.Write(this.Direction);
                writer.Write(this.State);

                return stream.ToArray();
            }

            public s_spindle FromArray(byte[] bytes)
            {
                var reader = new System.IO.BinaryReader(new System.IO.MemoryStream(bytes));
                this.Rpm = reader.ReadInt32();
                this.Direction = reader.ReadByte();
                this.State = reader.ReadByte();
                return this;
            }
        }

        public class s_motion
        {
            public byte Motion_Type;
            public float Feed_Rate;
            public UInt16 Feed_Rate_Mode;
            public UInt32 Line_Number;
            public float Axis_Values_0;
            public float Axis_Values_1;
            public float Axis_Values_2;
            public float Axis_Values_3;
            public float Axis_Values_4;
            public float Axis_Values_5;
            public float Horizontal_Center;
            public float Vertical_Center;
            public float Radius;
            public byte Block_Flag;


            public byte[] ToArray()
            {
                var stream = new System.IO.MemoryStream();
                var writer = new System.IO.BinaryWriter(stream);

                writer.Write(this.Motion_Type);
                writer.Write(this.Feed_Rate);
                writer.Write(this.Feed_Rate_Mode);
                writer.Write(this.Line_Number);
                writer.Write(this.Axis_Values_0);
                writer.Write(this.Axis_Values_1);
                writer.Write(this.Axis_Values_2);
                writer.Write(this.Axis_Values_3);
                writer.Write(this.Axis_Values_4);
                writer.Write(this.Axis_Values_5);
                writer.Write(this.Horizontal_Center);
                writer.Write(this.Vertical_Center);
                writer.Write(this.Radius);
                writer.Write(this.Block_Flag);

                return stream.ToArray();
            }

            public s_motion FromArray(byte[] bytes)
            {
                var reader = new System.IO.BinaryReader(new System.IO.MemoryStream(bytes));
                //this.Rpm = reader.ReadInt32();
                //this.Direction = reader.ReadByte();
                //this.State = reader.ReadByte();
                return this;
            }
        }
    }
}
