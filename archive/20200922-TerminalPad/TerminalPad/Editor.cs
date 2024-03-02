using System;
using System.Collections.Generic;
using System.IO;
using static System.Environment;
using static TerminalPad.Settings;
using static TerminalPad.Utils;

namespace TerminalPad
{
    public class MainEditor
    {
        public List<string> InlineFile = new List<string>();
        public string DirName = Directory.GetCurrentDirectory();
        public long LineCount = 1;
        public long Indent = 0;
        public string CodePrefix = "";
        public EditorSetting Setting;
        public MainEditor(EditorSetting Setting)
        {
            Console.Clear();
            PrintCol(this.Setting, "Terminal Code Editor [ Type .q to exit ]", ConsoleColor.Black, ConsoleColor.White, true);
            this.Setting = Setting;
        }

        public void Run()
        {
            while (true)
            {
                if (this.Setting.DisplayLineNumber)
                {
                    string DisplayPrefix = ProcessLineDisplay(this.LineCount);
                    PrintCol(this.Setting, $"{DisplayPrefix}{this.CodePrefix}", ConsoleColor.Green, ConsoleColor.Black, false);
                }
                else
                {
                    PrintCol(this.Setting, $"{this.CodePrefix}", ConsoleColor.Green, ConsoleColor.Black, false);
                }
                
                string Input = Console.ReadLine();
                if (Input.EndsWith("{") && this.Setting.EnableIndent)
                {
                    this.Indent += 1;
                }
                else if (Input.EndsWith("}") && this.Setting.EnableIndent)
                {
                    this.Indent -= 1;
                }
                else if (Input.StartsWith(".q"))
                {
                    this.Save();
                    break;
                }
                if (this.Indent < 0) this.Indent = 0;
                this.InlineFile.Add(this.CodePrefix + Input);
                this.CodePrefix = new string(' ', (int)(this.Setting.IndentCount * this.Indent));
                this.LineCount += 1;
            }
        }

        public string GetResult()
        {
            string Result = "";
            foreach (string Line in this.InlineFile)
            {
                Result += $"{Line}\n";
            }
            return Result;
        }

        public void Save()
        {
            string TextContent = this.GetResult();
            Console.Clear();
            if (this.Setting.AutoSave)
            {
                string Path = this.DirName + "\\" + this.Setting.SaveFile;
                if (File.Exists(Path))
                {
                    PrintCol(this.Setting, $"error: file name {this.Setting.SaveFile} already exists in {this.DirName}.", ConsoleColor.Red, ConsoleColor.Black, true);
                    Exit(1);
                }
                try
                {
                    File.WriteAllText(Path, TextContent);
                    PrintCol(this.Setting, $"success: {TextContent.Length} lines of text have been saved in file: {this.Setting.SaveFile} in {this.DirName}", ConsoleColor.Green, ConsoleColor.Black, true);
                }
                catch (Exception ERR)
                {
                    PrintCol(this.Setting, $"error on saving:\n{ERR}", ConsoleColor.Red, ConsoleColor.Black, true);
                }
                return;
            }
            PrintCol(this.Setting, $"Save as (in {this.DirName})\nType .a to abort and quit.\n>>> ", ConsoleColor.Green, ConsoleColor.Black, false);
            string FileName = Console.ReadLine();
            if (FileName.ToLower().StartsWith(".a")) { Exit(0); }
            if (File.Exists(this.DirName + "\\" + FileName))
            {
                PrintCol(this.Setting, $"error: file name {FileName} already exists in {this.DirName}.", ConsoleColor.Red, ConsoleColor.Black, true);
                Exit(1);
            }
            try
            {
                File.WriteAllText(this.DirName + "\\" + FileName, TextContent);
            }
            catch (Exception Error)
            {
                PrintCol(this.Setting, $"error: Oops. There was an error on saving the file:\n{Error}", ConsoleColor.Red, ConsoleColor.Black, true);
                Exit(1);
            }
        }
    }
}
