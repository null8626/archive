using System;
using static System.Environment;
using static TerminalPad.Utils;
using static TerminalPad.Settings;

namespace TerminalPad
{
    public class Program
    {
        public static void Main(string[] args)
        {
            EditorSetting Preset = (args.Length > 0) ? ParseArgument(args) : new EditorSetting();
            if (Preset == null) return;
            if (Preset.IndentCount < 0)
            {
                PrintCol(Preset, "error: indent below 0", ConsoleColor.Red, Console.BackgroundColor, true);
                Exit(1);
            }
            MainEditor editor = new MainEditor(Preset);
            editor.Run();
        }
    }
}