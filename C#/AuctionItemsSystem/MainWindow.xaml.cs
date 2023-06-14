using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace WpfDataBindingDemo
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly CollectionViewSource _listingDataView;

        public MainWindow()
        {
            InitializeComponent();
            _listingDataView = (CollectionViewSource)Resources["ListingDataView"];
        }

        private void OpenAddProduct_Click(object sender, RoutedEventArgs e)
        {
            var addProductWindow = new AddProductWindow();
            addProductWindow.ShowDialog();
        }

        private void Grouping_Checked(object sender, RoutedEventArgs e)
        {
            _listingDataView.GroupDescriptions.Add(new PropertyGroupDescription("Category"));
        }

        private void Grouping_Unchecked(object sender, RoutedEventArgs e)
        {
            _listingDataView.GroupDescriptions.Clear();
        }

        private void Sorting_Checked(object sender, RoutedEventArgs e)
        {
            _listingDataView.SortDescriptions.Add(
                new SortDescription("Category", ListSortDirection.Ascending));
            _listingDataView.SortDescriptions.Add(
                new SortDescription("StartDate", ListSortDirection.Ascending));
        }

        private void Sorting_Unchecked(object sender, RoutedEventArgs e)
        {
            _listingDataView.SortDescriptions.Clear();
        }
    }
}
