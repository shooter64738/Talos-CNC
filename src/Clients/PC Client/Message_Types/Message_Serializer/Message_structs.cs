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
    }
}
