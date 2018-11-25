using System;
using System.IO;
using System.Security.Cryptography;



class Checksum
{

    static string CalculateMD5(string filename)
    {
        using (var md5 = MD5.Create())
        {
            using (var stream = File.OpenRead(filename))
            {
                var hash = md5.ComputeHash(stream);
                return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
            }
        }
    }

    static string CalculateSHA1(string filename)
    {
        using (var sha1 = SHA1.Create())
        {
            using (var stream = File.OpenRead(filename))
            {
                var hash = sha1.ComputeHash(stream);
                return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
            }
        }
    }

    static string CalculateSHA256(string filename)
    {
        using (var sha256 = SHA256.Create())
        {
            using (var stream = File.OpenRead(filename))
            {
                var hash = sha256.ComputeHash(stream);
                return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
            }
        }
    }

    static string CalculateSHA512(string filename)
    {
        using (var sha512 = SHA512.Create())
        {
            using (var stream = File.OpenRead(filename))
            {
                var hash = sha512.ComputeHash(stream);
                return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
            }
        }
    }

    static void CalculateCheckSums(string filename)
    {
        Console.WriteLine("MD5 checksum: " + CalculateMD5(filename));
        Console.WriteLine("SHA-1 checksum: " + CalculateSHA1(filename));
        Console.WriteLine("SHA256 checksum: " + CalculateSHA256(filename));
        Console.WriteLine("SHA512 checksum: " + CalculateSHA512(filename));
    }

    static void Main(string[] args)
    {
        // Task 1
        CalculateCheckSums("npp.7.3.Installer.x64.exe");
    }
}
