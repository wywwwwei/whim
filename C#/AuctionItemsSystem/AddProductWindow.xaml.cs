using System;
using System.Collections.Generic;
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
using System.Windows.Shapes;

namespace WpfDataBindingDemo
{
    /// <summary>
    /// AddProductWindow.xaml 的交互逻辑
    /// </summary>
    public partial class AddProductWindow : Window
    {
        public AddProductWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            DataContext = new AuctionItem(ProductCategory.DvDs, "Type your description here", 1, DateTime.Now, SpecialFeatures.None);
        }

        private void ShowError(string message)
        {
            ErrorTextBlock.Visibility = Visibility.Visible;
            ErrorTextBlock.Text = message;
        }


        private void Submit_Click(object sender, RoutedEventArgs e)
        {
            if (DescriptionEntryForm.Text.Length == 0)
            {
                ShowError("Please, fill item description");
            }
            else if (StartPriceEntryForm.Text.Length == 0)
            {
                ShowError("Please, fill start price");
            }
            else if (Validation.GetHasError(StartPriceEntryForm))
            {
                ShowError("Please, enter a valid price");

            }
            else if (StartDateEntryForm.Text.Length == 0)
            {
                ShowError("Please, fill start date");
            }
            else if (Validation.GetHasError(StartDateEntryForm))
            {
                ShowError("Please, enter a valid date");
            }
            else
            {
                var item = (AuctionItem)(DataContext);
                ((App)Application.Current).AuctionItems.Add(item);
                Close();
            }
        }

        private void CategoryEntryForm_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        private void SpecialFeaturesEntryForm_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        private void DescriptionEntryForm_Error(object sender, ValidationErrorEventArgs e)
        {

        }

        private void StartPriceEntryForm_Error(object sender, ValidationErrorEventArgs e)
        {

        }

        private void StartDateEntryForm_Error(object sender, ValidationErrorEventArgs e)
        {

        }
    }
}
