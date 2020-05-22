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

namespace UI.Views
{
    /// <summary>
    /// Interaction logic for UAT_Settings_Control.xaml
    /// </summary>
    public partial class UI_Physical_Machine_Settings: UserControl
    {
        public UI_Physical_Machine_Settings()
        {
            InitializeComponent();
        }

        private void close_button_CustomClick(object sender, RoutedEventArgs e)
        {
            UI.MainWindow.childContainer.Children.Clear();
        }
    }
}
