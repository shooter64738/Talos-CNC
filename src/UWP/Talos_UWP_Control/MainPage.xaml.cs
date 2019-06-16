using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace Talos_UWP_Control
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {

        public static class Configuration
        {
            public static string[] Machine_Axis_Names = new string[9] { "X", "Y", "Z", "A", "B", "C", "U", "V", "W" };
            public static int MACHINE_AXIS_COUNT = 8;
            public static SolidColorBrush MachineCoordForeBrush = new SolidColorBrush(Windows.UI.Colors.SlateBlue);
            public static SolidColorBrush ForeBrush = new SolidColorBrush(Windows.UI.Colors.Green);
            public static SolidColorBrush BackBrush = new SolidColorBrush(Windows.UI.Colors.Black);
            public static float AxisMachineFontSize = 16;
            public static float AxisWorkFontSize = 20;
            public static float AxisIDFontSize = 20;
        }
        public MainPage()
        {
            this.InitializeComponent();
            mainGrid.Background = Configuration.BackBrush;
            axis_grid.Background = null;
            gcode_text.Text = "G17 G20 G90 G94 G54\rG0 Z0.25\rX-0.5 Y0.\rZ0.1\rG01 Z0. F5.\rG02 X0. Y0.5 I0.5 J0.F2.5\r";
            gcode_text.Text += "X0.5 Y0.I0.J-0.5\rX0.Y-0.5 I-0.5 J0.\rX-0.5 Y0.I0.J0.5\rG01 Z0.1 F5.\rG00 X0. Y0.Z0.25\r";
            gcode_text.Text += "G17 G20 G90 G94 G54\rG0 Z0.25\rX-0.5 Y0.\rZ0.1\rG01 Z0. F5.\rG02 X0. Y0.5 I0.5 J0.F2.5\r";
            gcode_text.Text += "X0.5 Y0.I0.J-0.5\rX0.Y-0.5 I-0.5 J0.\rX-0.5 Y0.I0.J0.5\rG01 Z0.1 F5.\rG00 X0. Y0.Z0.25\r";
            Build_Coord_Display();


        }
        public void Build_Coord_Display()
        {
            coordinateGrid.Background = Configuration.BackBrush;

            for (int i = 0; i < Configuration.MACHINE_AXIS_COUNT; i++)
            {
                AddAxisToGrid(i);
                ////Grid itemgrid = new Grid();
                ////itemgrid.Margin = new Thickness(4);
                ////RowDefinition itemRow = new RowDefinition();
                ////itemRow.Height = new GridLength(1, GridUnitType.Auto);
                ////itemgrid.RowDefinitions.Add(new RowDefinition());

                ////for (int ii = 0; ii < 3; ii++)
                ////{
                ////    ColumnDefinition itemCol = new ColumnDefinition();
                ////    itemCol.Width = new GridLength(1, GridUnitType.Star);
                ////    itemgrid.ColumnDefinitions.Add(itemCol);
                ////}
                //Grid itemgrid = MakeAxisItem();
                //itemgrid.Margin = new Thickness(4);

                //RowDefinition newRow = new RowDefinition();
                //newRow.Height = new GridLength(1, GridUnitType.Auto);
                //axis_grid.RowDefinitions.Add(new RowDefinition());

                //TextBlock axisid = new TextBlock();
                //axisid.Text = Configuration.Machine_Axis_Names[i];
                //axisid.HorizontalAlignment = HorizontalAlignment.Left;
                //axisid.VerticalAlignment = VerticalAlignment.Center;
                //axisid.SetValue(Grid.RowProperty, i);
                //axisid.SetValue(Grid.ColumnProperty, 0);
                //axisid.SetValue(TextBlock.FontWeightProperty, FontWeights.Bold);
                //axisid.SetValue(TextBlock.FontSizeProperty, Configuration.AxisFontSize);
                //axisid.Foreground = Configuration.ForeBrush;

                //TextBlock axisvalue = new TextBlock();
                //axisvalue.Text = "9,876,543.2109";
                //axisvalue.HorizontalAlignment = HorizontalAlignment.Stretch;
                //axisvalue.HorizontalTextAlignment = TextAlignment.Right;
                //axisvalue.VerticalAlignment = VerticalAlignment.Center;
                //axisvalue.SetValue(Grid.RowProperty, i);
                //axisvalue.SetValue(Grid.ColumnProperty, 1);
                //axisvalue.SetValue(TextBlock.FontWeightProperty, FontWeights.Bold);
                //axisvalue.SetValue(TextBlock.FontSizeProperty, Configuration.AxisFontSize);
                //axisvalue.Foreground = Configuration.ForeBrush;


                //TextBlock axisunit = new TextBlock();
                //axisunit.Text = "mm";
                //axisunit.HorizontalAlignment = HorizontalAlignment.Right;
                //axisunit.VerticalAlignment = VerticalAlignment.Center;
                //axisunit.SetValue(Grid.RowProperty, i);
                //axisunit.SetValue(Grid.ColumnProperty, 2);
                //axisunit.SetValue(TextBlock.FontWeightProperty, FontWeights.Bold);
                //axisunit.SetValue(TextBlock.FontSizeProperty, Configuration.AxisFontSize);
                //axisunit.Foreground = Configuration.ForeBrush;

                //itemgrid.Children.Add(axisid);
                //itemgrid.Children.Add(axisvalue);
                //itemgrid.Children.Add(axisunit);

                //Border axisborder = new Border();
                //axisborder.BorderBrush = Configuration.ForeBrush; ;
                //axisborder.CornerRadius = new CornerRadius(4);
                //axisborder.BorderThickness = new Thickness(1);
                //axisborder.Margin = new Thickness(4);
                //axisborder.Child = itemgrid;

                //axisborder.SetValue(Grid.RowProperty, i);
                //axisborder.SetValue(Grid.ColumnProperty, 0);

                //axis_grid.Children.Add(axisborder);

                ////axis_grid.Children.Add(axisid);
                ////axis_grid.Children.Add(axisvalue);
                ////axis_grid.Children.Add(axisunit);
            }

        }

        private void AddAxisToGrid(int axis_number)
        {
            TextBlock axis_id = new TextBlock();
            axis_id.FontSize = Configuration.AxisIDFontSize;
            axis_id.SetValue(Grid.ColumnProperty, 0);
            axis_id.SetValue(Grid.RowProperty, axis_number);
            axis_id.Text = Configuration.Machine_Axis_Names[axis_number];
            axis_id.Foreground = Configuration.ForeBrush;
            coordinateGrid.Children.Add(axis_id);

            //Work coordinate textbox
            TextBlock work_coord_textbox = new TextBlock();
            work_coord_textbox.Text = "0000.000";
            work_coord_textbox.FontSize = Configuration.AxisWorkFontSize;
            work_coord_textbox.Foreground = Configuration.ForeBrush;
            work_coord_textbox.HorizontalTextAlignment = TextAlignment.Right;
            work_coord_textbox.Margin = new Thickness(2);

            //Work coordinate units
            TextBlock work_coord_units = new TextBlock();
            work_coord_units.Text = "mm";
            work_coord_units.FontSize = Configuration.AxisWorkFontSize;
            work_coord_units.Foreground = Configuration.ForeBrush;
            work_coord_units.HorizontalTextAlignment = TextAlignment.Right;
            //work_coord_units.Margin = new Thickness(2);

            //Work coord stack panel
            StackPanel work_panel = new StackPanel();
            work_panel.Orientation = Orientation.Horizontal;
            work_panel.Children.Add(work_coord_textbox);
            work_panel.Children.Add(work_coord_units);

            //Work coordinate border
            Border coordBorder = new Border();
            coordBorder.BorderBrush = Configuration.ForeBrush; ;
            coordBorder.CornerRadius = new CornerRadius(4);
            coordBorder.BorderThickness = new Thickness(1);
            coordBorder.Margin = new Thickness(4);
            coordBorder.Child = work_panel;
            coordBorder.SetValue(Grid.ColumnProperty, 1);
            coordBorder.SetValue(Grid.RowProperty, axis_number);
            coordBorder.BorderBrush = Configuration.ForeBrush;
            coordinateGrid.Children.Add(coordBorder);

            //Machine coordinate textblock
            TextBlock machine_coord_textblock = new TextBlock();
            machine_coord_textblock.Text = "0000.000";
            machine_coord_textblock.FontSize = Configuration.AxisMachineFontSize; ;
            machine_coord_textblock.SetValue(Grid.ColumnProperty, 2);
            machine_coord_textblock.SetValue(Grid.RowProperty, axis_number);
            machine_coord_textblock.Foreground = Configuration.MachineCoordForeBrush;
            machine_coord_textblock.HorizontalTextAlignment = TextAlignment.Right;
            coordinateGrid.Children.Add(machine_coord_textblock);

        }
    }
}
