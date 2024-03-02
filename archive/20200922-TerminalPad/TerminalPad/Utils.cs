using System;
using static System.Environment;
using static TerminalPad.Settings;

namespace TerminalPad
{
    public class Utils
    {
        public static string ProcessLineDisplay(long Number)
        {
            if (Number > 999999) { Exit(0); }
            int TempLength = 5 - Number.ToString().Length;
            if (TempLength < 0) TempLength = 0;
            return $"{new string(' ', TempLength)}{Number} | ";
        }
        public static void PrintCol(EditorSetting Setting, string Content, ConsoleColor Foreground, ConsoleColor Background, bool Line)
        {
            if ((Setting != null) && (Setting.HaveColors == false))
            {
                if (Line)
                {
                    Console.WriteLine(Content);
                    return;
                }
                Console.Write(Content);
                return;
            }
            ConsoleColor DefaultFG = Console.ForegroundColor;
            ConsoleColor DefaultBG = Console.BackgroundColor;
            Console.ForegroundColor = Foreground;
            Console.BackgroundColor = Background;
            if (Line)
            {
                Console.WriteLine(Content, Console.ForegroundColor, Console.BackgroundColor);
            }
            else
            {
                Console.Write(Content, Console.ForegroundColor, Console.BackgroundColor);
            }
            Console.ForegroundColor = DefaultFG;
            Console.BackgroundColor = DefaultBG;
        }
    }
}
