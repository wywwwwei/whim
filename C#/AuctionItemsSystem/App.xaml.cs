using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace WpfDataBindingDemo
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public ObservableCollection<AuctionItem> AuctionItems { get; set; } = new ObservableCollection<AuctionItem>();

        private void Application_Startup(object sender, StartupEventArgs e)
        {
            LoadAuctionData();
        }

        private void LoadAuctionData()
        {
            var camera = new AuctionItem(ProductCategory.Electronics, "Digital camera - good condition", 300, new DateTime(2023, 6, 15), SpecialFeatures.None);
            var snowBoard = new AuctionItem(ProductCategory.Sports, "Snowboard and bindings", 120, new DateTime(2023, 6, 14), SpecialFeatures.None);
            var computer = new AuctionItem(ProductCategory.Electronics, "Computer - good condition", 1000, new DateTime(2023, 6, 19), SpecialFeatures.None);
            AuctionItems.Add(camera);
            AuctionItems.Add(snowBoard);
            AuctionItems.Add(computer);
        }
    }
}
