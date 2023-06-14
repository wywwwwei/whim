using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace WpfDataBindingDemo
{
    internal class FutureDateRule : ValidationRule
    {
        public override ValidationResult Validate(object value, CultureInfo cultureInfo)
        {
            var dateStr = value.ToString();
            if (String.IsNullOrEmpty(dateStr))
            {
                return new ValidationResult(false, "Value is empty");
            }
            DateTime date;
            try
            {
                date = DateTime.Parse(dateStr);
            }
            catch (FormatException)
            {
                return new ValidationResult(false, "Value is not a valid date. Please enter a valid date");
            }
            if (date < DateTime.Now.Date)
            {
                return new ValidationResult(false, "Value is not a future date. Please enter a date in the future.");
            }
            return ValidationResult.ValidResult;
        }
    }
}
