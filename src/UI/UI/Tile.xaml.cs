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

namespace UI
{
    /// <summary>
    /// Interaction logic for Tile.xaml
    /// </summary>
    public partial class Tile : UserControl
    {
        public Tile()
        {
            InitializeComponent();
            this.DataContext = this;
        }
        public event RoutedEventHandler CustomClick;
        private void UserControl_MouseEnter(object sender, MouseEventArgs e)
        {
            if (this.IsEnabled)
                CaptureMouse();
        }

        private void UserControl_MouseMove(object sender, MouseEventArgs e)
        {
            Point mouseposition = e.GetPosition(this);
            if (mouseposition.X < 0 || mouseposition.Y < 0 || mouseposition.X > this.ActualWidth || mouseposition.Y > this.ActualHeight)
                ReleaseMouseCapture();
        }

        private void UserControl_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (CustomClick != null)
            {
                CustomClick(this, new RoutedEventArgs());
            }
        }

        public string Description
        {
            get { return (string)GetValue(DescriptionProperty); }
            set { SetValue(DescriptionProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Property1.  
        // This enables animation, styling, binding, etc...
        public static readonly DependencyProperty DescriptionProperty
            = DependencyProperty.Register(
                  "Description",
                  typeof(string),
                  typeof(Tile),
                  new PropertyMetadata("DescriptionPropertyValue")
              );

        public string Title
        {
            get { return (string)GetValue(TitleProperty); }
            set { SetValue(TitleProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Property1.  
        // This enables animation, styling, binding, etc...
        public static readonly DependencyProperty TitleProperty
            = DependencyProperty.Register(
                  "Title",
                  typeof(string),
                  typeof(Tile),
                  new PropertyMetadata("TitlePropertyValue")
              );

        public string Icon
        {
            get
            {
                string value = (string)GetValue(IconProperty);
                Uri iuri = new Uri(value);
                iconImage.Source = new BitmapImage(iuri);
                return value;
            }
            set
            {
                SetValue(IconProperty, "icons/" + value);
                iconImage.Source = new BitmapImage(new Uri("icons/" + value, UriKind.Relative));
            }
        }

        // Using a DependencyProperty as the backing store for Property1.  
        // This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IconProperty
            = DependencyProperty.Register(
                  "Icon",
                  typeof(string),
                  typeof(Tile),
                  new PropertyMetadata("IconPropertyValue")
              );
    }
}
