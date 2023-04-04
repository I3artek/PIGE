using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Reflection;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GradedLab2
{
    public enum Language
    {
        [Description("EN")] Debug,
        [Description("EN")] English,
        [Description("PL")] Polish,
        [Description("ES")] Spanish,
        [Description("PT")] Portuguese,
        [Description("ZH")] Chinese,
    }

    public static class Enumeration
    {
        public static string GetDescription(this Enum value)
        {
            FieldInfo fi = value.GetType().GetField(value.ToString());

            DescriptionAttribute[] attributes = fi.GetCustomAttributes(typeof(DescriptionAttribute), false) as DescriptionAttribute[];

            if (attributes != null && attributes.Any())
            {
                return attributes.First().Description;
            }

            return value.ToString();
        }
    }

    public class LocEntry
    {
        public LocEntry(string hierarchyPath, string entryName)
        {
            EntryName = entryName;
            HierarchyPath = hierarchyPath;

            Translations = new Dictionary<Language, string>();

            LocKey = $"{HierarchyPath}-{EntryName}".Hash();
        }

        public string LocKey { get; private set; }

        public string HierarchyPath { get; private set; }

        public string EntryName { get; private set; }

        public Dictionary<Language, string> Translations { get; set; }

        public override string ToString()
        {
            return $"LocKey#{LocKey}";
        }
    }

    public static class StringHash
    {
        static readonly int MaxBytes = 4;

        public static string Hash(this string locEntry)
        {
            using (SHA256 sHA256 = SHA256.Create())
            {
                byte[] bytes = sHA256.ComputeHash(Encoding.UTF8.GetBytes(locEntry));

                StringBuilder builder = new StringBuilder();

                for (int i = 0; i < MaxBytes; i++)
                {
                    builder.Append(bytes[i].ToString("x2"));
                }

                return builder.ToString();
            }
        }
    }

    public static class Translator
    {
        public static string Translate(string languageFrom, string languateTo, string text)
        {
            HttpClient client = new HttpClient();
            {
                HttpRequestMessage requestMessage = new(HttpMethod.Get, $"https://translate.googleapis.com/translate_a/single?client=gtx&sl={languageFrom}&tl={languateTo}&dt=t&q={text}");

                HttpResponseMessage responseMessage = client.Send(requestMessage);

                if (responseMessage.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    string translationContent = responseMessage.Content.ReadAsStringAsync().Result;

                    int firstApostrophe = translationContent.IndexOf("\""); int secondApostrophe = translationContent.IndexOf("\"", firstApostrophe + 1);

                    return translationContent.Substring(firstApostrophe + 1, secondApostrophe - firstApostrophe - 1);
                }
            }

            return string.Empty;
        }
    }
}
