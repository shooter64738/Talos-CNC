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
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public static Grid childContainer;
        public static WrapPanel parentContainer;
        public static UI.ViewModels.Physical_Machine_Settings UI_Settings = new UI.ViewModels.Physical_Machine_Settings();

        SolidColorBrush machine_configuration_color = new SolidColorBrush(Color.FromArgb(255, 255, 150, 0));

        List<string> tile_objects = new List<string>();
        public MainWindow()
        {
            InitializeComponent();
            childContainer = this.childGrid;
            parentContainer = this.parentGrid;
            tile_objects.Add(typeof(UI.Views.UI_Physical_Machine_Settings).ToString());

        }

        private void UI_Settings_Click(object sender, RoutedEventArgs e)
        {
            UI.Views.UI_Physical_Machine_Settings newWindow = new Views.UI_Physical_Machine_Settings();
            showChild(newWindow, UI_Settings);
        }

        private void showParent(UserControl newWindow, object DataClass)
        {
            baseGrid.Children.Remove(securityWarning);

            newWindow.HorizontalAlignment = HorizontalAlignment.Stretch;
            newWindow.VerticalAlignment = VerticalAlignment.Stretch;
            newWindow.Visibility = Visibility.Visible;

            newWindow.DataContext = DataClass;

            parentContainer.Children.Add(newWindow);
        }

        private void showChild(UserControl newWindow, object DataClass)
        {
            newWindow.HorizontalAlignment = HorizontalAlignment.Stretch;
            newWindow.VerticalAlignment = VerticalAlignment.Stretch;
            newWindow.Visibility = Visibility.Visible;

            newWindow.DataContext = DataClass;

            childContainer.Children.Add(newWindow);
        }

        private void Grid_Loaded(object sender, RoutedEventArgs e)
        {
            {
                Tile newTile = new Tile();
                newTile.Height = 200;
                newTile.Width = 350;
                showParent(newTile, newTile);
                newTile.Background = machine_configuration_color;
                newTile.Margin = new Thickness(15);
                newTile.CustomClick += UI_Settings_Click;
                newTile.Description = "Specify the machines physical parameters. Machine type, axis distance, etc..";
                newTile.Title = "Physical Machine Settings";
                newTile.Icon = "settings.png";
            }

            {
                Tile newTile = new Tile();
                newTile.Height = 200;
                newTile.Width = 350;
                showParent(newTile, newTile);
                newTile.Background = machine_configuration_color;
                newTile.Margin = new Thickness(15);
                newTile.CustomClick += UI_Settings_Click;
                newTile.Description = "Specify the machine virtual parameters such as steps per unit, number of axis, backlash compensation, etc..";
                newTile.Title = "Virtual Machine Configuration";
                newTile.Icon = "settings.png";
            }
        }
    }
}
