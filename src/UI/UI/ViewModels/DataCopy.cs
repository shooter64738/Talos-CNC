using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace UI.ViewModels
{
    public class DataCopy : UI.ViewModels.PropertyNotifier
    {
        public DataCopy()
        {
            _source = "data source";
            _destination = "data destination";
        }
        private string _source;
        public string source
        {
            get { return _source; }
            set { _source = value; OnPropertyChanged("source"); }
        }

        private string _destination;
        public string destination
        {
            get { return _destination; }
            set { _destination = value; OnPropertyChanged("destination"); }
        }

    }
}
