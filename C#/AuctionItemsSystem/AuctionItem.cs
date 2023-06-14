using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WpfDataBindingDemo
{
    public enum ProductCategory
    {
        Books,
        Computers,
        DvDs,
        Electronics,
        Home,
        Sports
    }

    public enum SpecialFeatures
    {
        None,
        Color,
        Highlight
    }

    public class AuctionItem : INotifyPropertyChanged
    {
        private ProductCategory _category;
        private string _description;
        private int _startPrice;
        private DateTime _startDate;
        private SpecialFeatures _specialFeatures;

        public event PropertyChangedEventHandler? PropertyChanged;

        public AuctionItem(ProductCategory category, string description, int startPrice, DateTime startDate, SpecialFeatures specialFeatures)
        {
            _category = category;
            _description = description;
            _startPrice = startPrice;
            _startDate = startDate;
            _specialFeatures = specialFeatures;
        }

        protected void OnPropertyChanged(string name)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }

        #region Properties Getters and Setters

        public ProductCategory Category
        {
            get { return _category; }
            set
            {
                _category = value;
                OnPropertyChanged("Category");
            }
        }

        public string Description
        {
            get { return _description; }
            set
            {
                _description = value;
                OnPropertyChanged("Description");
            }
        }

        public int StartPrice
        {
            get { return _startPrice; }
            set
            {
                _startPrice = value;
                OnPropertyChanged("StartPrice");
            }
        }

        public DateTime StartDate
        {
            get { return _startDate; }
            set
            {
                _startDate = value;
                OnPropertyChanged("StartDate");
            }
        }

        public SpecialFeatures SpecialFeatures
        {
            get { return _specialFeatures; }
            set
            {
                _specialFeatures = value;
                OnPropertyChanged("SpecialFeatures");
            }
        }

        #endregion
    }
}
