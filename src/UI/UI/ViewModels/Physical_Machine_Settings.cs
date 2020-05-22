using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using System.Windows.Data;

namespace UI.ViewModels
{
    public class Physical_Machine_Settings : UI.ViewModels.PropertyNotifier
    {
        public Physical_Machine_Settings()
        {
            _Machine_Type = "Vertical Milling Center";
            _Axis_Count = 0;

            _Axis_List = new ObservableCollection<axis_limits>();

            _Axis_List.Add(new axis_limits());
            _Axis_List.Add(new axis_limits());
            _Axis_List.Add(new axis_limits());

            _Axis_List.Add(new axis_limits());
            _Axis_List.Add(new axis_limits());
            _Axis_List.Add(new axis_limits());

            _Axis_List.Add(new axis_limits());
            _Axis_List.Add(new axis_limits());
            _Axis_List.Add(new axis_limits());

            _Axis_List.Add(new axis_limits());
        }

        private void reset_selections()
        {
            foreach (axis_limits item in _Axis_List)
            {
                item.Name = "";
                item.Distance = 0;
            }
        }

        ObservableCollection<string> _Available_Axis_Names = new ObservableCollection<string> { "", "A", "B", "C", "X", "Y", "Z", "U", "V", "W", "E" };
        public ObservableCollection<string> Available_Axis_Names
        {
            get { return _Available_Axis_Names; }
            set { _Available_Axis_Names = value; }
        }

        ObservableCollection<axis_limits> _Axis_List;
        public ObservableCollection<axis_limits> Axis_List
        {
            get { return _Axis_List; }
            set { _Axis_List = value; OnPropertyChanged("Axis_List"); }
        }

        private string _Machine_Type;
        public string Machine_Type
        {
            get { return _Machine_Type; }
            set
            {
                //Only allow E axis on 3d printers for extruder
                if (value != "Fusion Deposit Material (3d Printer)")
                {
                    if (_Available_Axis_Names.Contains("E"))
                    {
                        _Available_Axis_Names.Remove("E");
                        reset_selections();
                    }
                }
                else
                {
                    if (!_Available_Axis_Names.Contains("E"))
                    {
                        _Available_Axis_Names.Add("E");
                        reset_selections();
                    }
                }

                _Machine_Type = value;
                OnPropertyChanged("Machine_Type");
            }
        }

        private int _Axis_Count;
        public int Axis_Count
        {
            get { return _Axis_Count; }
            set { _Axis_Count = value; OnPropertyChanged("Axis_Count"); }
        }    

        public class axis_limits : UI.ViewModels.PropertyNotifier
        {

            public axis_limits()
            { }

            public axis_limits(string name, double distance, bool selected)
            {
                _Name = name;
                _Distance = distance;
            }
            
            private string _Name;
            public string Name
            {
                get { return _Name; }
                set { _Name = value; OnPropertyChanged("Name"); }
            }

            private double _Distance;
            public double Distance
            {
                get { return _Distance; }
                set { _Distance = value; OnPropertyChanged("Distance"); }

            }
        }
    }
}