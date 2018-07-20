using System;
using System.Security.Cryptography;
using System.Text;

public class PasswordGenerator
{
    public PasswordGenerator()
    {
        this.Minimum = DefaultMinimum;
        this.Maximum = DefaultMaximum;
        this.ConsecutiveCharacters = false;
        this.RepeatCharacters = true;
        this.ExcludeSymbols = false;
        this.Exclusions = null;

        rng = new RNGCryptoServiceProvider();
    }

    protected int GetCryptographicRandomNumber(int lBound, int uBound)
    {
        // Assumes lBound >= 0 && lBound < uBound
        // returns an int >= lBound and < uBound
        uint urndnum;
        byte[] rndnum = new Byte[4];
        if (lBound == uBound - 1)
        {
            // test for degenerate case where only lBound can be returned
            return lBound;
        }

        uint xcludeRndBase = (uint.MaxValue -
            (uint.MaxValue % (uint)(uBound - lBound)));

        do
        {
            rng.GetBytes(rndnum);
            urndnum = System.BitConverter.ToUInt32(rndnum, 0);
        } while (urndnum >= xcludeRndBase);

        return (int)(urndnum % (uBound - lBound)) + lBound;
    }

    protected char GetRandomCharacter()
    {
        int upperBound = pwdCharArray.GetUpperBound(0);

        int randomCharPosition = GetCryptographicRandomNumber(
            pwdCharArray.GetLowerBound(0), upperBound);

        char randomChar = pwdCharArray[randomCharPosition];

        return randomChar;
    }

    public string Generate()
    {
        // Pick random length between minimum and maximum   
        int pwdLength = 20;

        StringBuilder pwdBuffer = new StringBuilder();
        pwdBuffer.Capacity = this.Maximum;

        // Generate random characters
        char lastCharacter, nextCharacter;

        // Initial dummy character flag
        lastCharacter = nextCharacter = '\n';

        for (int i = 0; i < pwdLength; i++)
        {
            nextCharacter = GetRandomCharacter();

            if (false == this.ConsecutiveCharacters)
            {
                while (lastCharacter == nextCharacter)
                {
                    nextCharacter = GetRandomCharacter();
                }
            }

            if (false == this.RepeatCharacters)
            {
                string temp = pwdBuffer.ToString();
                int duplicateIndex = temp.IndexOf(nextCharacter);
                while (-1 != duplicateIndex)
                {
                    nextCharacter = GetRandomCharacter();
                    duplicateIndex = temp.IndexOf(nextCharacter);
                }
            }

            if ((null != this.Exclusions))
            {
                while (-1 != this.Exclusions.IndexOf(nextCharacter))
                {
                    nextCharacter = GetRandomCharacter();
                }
            }

            pwdBuffer.Append(nextCharacter);
            lastCharacter = nextCharacter;
        }

        if (null != pwdBuffer)
        {
            return pwdBuffer.ToString();
        }
        else
        {
            return String.Empty;
        }
    }

    public string Exclusions
    {
        get { return this.exclusionSet; }
        set { this.exclusionSet = value; }
    }

    public int Minimum
    {
        get { return this.minSize; }
        set
        {
            this.minSize = value;
            if (PasswordGenerator.DefaultMinimum > this.minSize)
            {
                this.minSize = PasswordGenerator.DefaultMinimum;
            }
        }
    }

    public int Maximum
    {
        get { return this.maxSize; }
        set
        {
            this.maxSize = value;
            if (this.minSize >= this.maxSize)
            {
                this.maxSize = PasswordGenerator.DefaultMaximum;
            }
        }
    }

    public bool ExcludeSymbols
    {
        get { return this.hasSymbols; }
        set { this.hasSymbols = value; }
    }

    public bool RepeatCharacters
    {
        get { return this.hasRepeating; }
        set { this.hasRepeating = value; }
    }

    public bool ConsecutiveCharacters
    {
        get { return this.hasConsecutive; }
        set { this.hasConsecutive = value; }
    }

    private const int DefaultMinimum = 6;
    private const int DefaultMaximum = 10;
    private const int UBoundDigit = 61;

    private RNGCryptoServiceProvider rng;
    private int minSize;
    private int maxSize;
    private bool hasRepeating;
    private bool hasConsecutive;
    private bool hasSymbols;
    private string exclusionSet;
    private char[] pwdCharArray = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789".ToCharArray();
}