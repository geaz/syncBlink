using System;
using System.Windows.Media;

namespace SyncBlink.Desktop
{
    public static class ColorExtensions
    {
        public static Color ColorFromHSV(double hue, double saturation, double value)
        {
            int hi = Convert.ToByte(Math.Floor(hue / 60)) % 6;
            double f = hue / 60 - Math.Floor(hue / 60);

            value = value * 255;
            int v = Convert.ToInt32(value);
            int p = Convert.ToInt32(value * (1 - saturation));
            int q = Convert.ToInt32(value * (1 - f * saturation));
            int t = Convert.ToInt32(value * (1 - (1 - f) * saturation));

            if (hi == 0)
                return Color.FromScRgb(255, v, t, p);
            else if (hi == 1)
                return Color.FromScRgb(255, q, v, p);
            else if (hi == 2)
                return Color.FromScRgb(255, p, v, t);
            else if (hi == 3)
                return Color.FromScRgb(255, p, q, v);
            else if (hi == 4)
                return Color.FromScRgb(255, t, p, v);
            else
                return Color.FromScRgb(255, v, p, q);
        }
    }
}
