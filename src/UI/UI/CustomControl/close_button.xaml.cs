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

namespace UI.CustomControl
{
    /// <summary>
    /// Interaction logic for close_button.xaml
    /// </summary>
    public partial class close_button : UserControl
    {
        public close_button()
        {
            InitializeComponent();
        }

        public event RoutedEventHandler CustomClick;
        private void Image_MouseEnter(object sender, MouseEventArgs e)
        {
            if (this.IsEnabled)
                CaptureMouse();
        }

        private void Image_MouseMove(object sender, MouseEventArgs e)
        {
            Point mouseposition = e.GetPosition(this);
            if (mouseposition.X < 0 || mouseposition.Y < 0 || mouseposition.X > this.ActualWidth || mouseposition.Y > this.ActualHeight)
                ReleaseMouseCapture();
        }

        private void Image_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (CustomClick != null)
            {
                CustomClick(this, new RoutedEventArgs());
            }
        }
    }
}
