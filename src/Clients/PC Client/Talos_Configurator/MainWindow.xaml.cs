using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Talos_PC_Client
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {


            InitializeComponent();
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            //Spindle
            //tester for the binary message protocol. 
            //Message_Serializer.Message_structs.s_spindle spindle = new Message_Serializer.Message_structs.s_spindle();
            Message_Serializer.Message_structs.s_motion spindle = new Message_Serializer.Message_structs.s_motion();
            Serial_Library.Comm.Open("COM6", 115200, 55, spindle);

            //while (true)
            //{
            //    if (Serial_Library.Comm.Ports[0].MessageCount > 0)
            //    {
            //        float rpm = ((Message_Serializer.Message_structs.s_spindle)
            //            Serial_Library.Comm.Ports[0].Get_Message_Serialized(Serial_Library.Comm.Ports[0].MessageCount - 1)).Rpm;
            //        Serial_Library.Comm.Ports[0].Drop_Message(Serial_Library.Comm.Ports[0].MessageCount - 1);
            //    }
            //}

            //Motion
            spindle.Motion_Type = 1;
            spindle.Feed_Rate_Mode = 9400;
            spindle.Feed_Rate = 5000;
            spindle.Axis_Values_0 = 10; spindle.Axis_Values_1 = 10; spindle.Axis_Values_2 = 10;
            spindle.Axis_Values_3 = 10; spindle.Axis_Values_4 = 10; spindle.Axis_Values_5 = 10;
            spindle.Line_Number = 1;

            Serial_Library.Comm.Ports[0].Send_Message_Serialized(spindle.ToArray());


        }
    }
}
