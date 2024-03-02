using System;
using System.Collections.Generic;
using static TerminalPad.Utils;
using static System.Environment;

namespace TerminalPad
{
    public class Settings
    {
        public class HelpCommand
        {
            private string ProcessText(string Param)
            {
                string[] ByQuestionMark = Param.Split('?');
                string HelpName = string.Empty;
                char[] NameChars = ByQuestionMark[0].ToCharArray();
                for (int i = 0; i < 30; i++)
                {
                    if (i >= NameChars.Length) { HelpName += ' '; } else
                    {
                        HelpName += NameChars[i];
                    }
                }
                return HelpName + ByQuestionMark[1];
            }
            public string[] Text;
            public bool Shown = false;
            public HelpCommand()
            {
                string[] Text = {
                    "--help?Shows this help command.",
                    "--colorless?Makes the editor color less.",
                    "--indent <number>?Shows how many indents should be added once every { or } is added.",
                    "--disableindent?Disables the automatic indentation.",
                    "--nolinecount?Disables showing the line count in the editor.",
                    "--saveto <filename>?Saves instantly to a file after inputting '.q'."
                };
                this.Text = Text;
            }
            public void Show()
            {
                PrintCol(new EditorSetting(), "TerminalPad Help (and their parameters)\n", ConsoleColor.Green, ConsoleColor.Black, true);
                foreach (string Parameter in this.Text)
                {
                    string Processed = ProcessText(Parameter);
                    Console.WriteLine(Processed);
                }
                this.Shown = true;
            }
        }
        public class EditorSetting
        {
            public long IndentCount = 4;
            public string SaveFile = null;
            public bool AutoSave = false;
            public bool EnableIndent = true;
            public bool HaveColors = true;
            public bool DisplayLineNumber = true;
        }
        public static EditorSetting ParseArgument(string[] Args)
        {
            int TempCount = 0;
            List<string> Parsed = new List<string>(Args);
            EditorSetting Setting = new EditorSetting();
            HelpCommand Help = new HelpCommand();
            foreach (string arg in Parsed)
            {
                switch (arg.ToLower())
                {
                    case "--indent":
                        long IndentCount = 4;
                        try
                        {
                            IndentCount = Convert.ToInt64(Parsed[TempCount + 1]);
                        }
                        catch (Exception ERR)
                        {
                            PrintCol(Setting, $"error: invalid --indent parameter. must be a number\nFull detailed error: {ERR}", ConsoleColor.Red, Console.BackgroundColor, true);
                            Exit(1);
                        }
                        Setting.IndentCount = IndentCount;
                        break;
                    case "--disableindent":
                        Setting.IndentCount = 0;
                        Setting.EnableIndent = false;
                        break;
                    case "--colorless":
                        Setting.HaveColors = false;
                        break;
                    case "--nolinecount":
                        Setting.DisplayLineNumber = false;
                        break;
                    case "--saveto":
                        Setting.AutoSave = true;
                        try
                        {
                            Setting.SaveFile = Parsed[TempCount + 1];
                        }
                        catch
                        {
                            PrintCol(Setting, "error: please input a file to save after the --saveto parameter.", ConsoleColor.Red, ConsoleColor.Black, true);
                            Exit(1);
                        }
                        break;
                    case "--help":
                        Help.Show();
                        break;
                    case "--h":
                        Help.Show();
                        break;
                    case "-h":
                        Help.Show();
                        break;
                    case "-help":
                        Help.Show();
                        break;
                    case "/?":
                        Help.Show();
                        break;
                    default:
                        break;
                }
                if (Help.Shown) return null;
                TempCount += 1;
            }
            return Setting;
        }
    }
}
