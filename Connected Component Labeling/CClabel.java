import java.io.File;
import java.io.FileWriter;
import java.util.Scanner;

public class CClabel
{
    public int numRows, numCols, minVal, maxVal;
    public int newLabel, trueNumCC, newMin, newMax;
    public int zeroFramedAry[][], NonZeroNeighborAry[], EQAry[];
    public Property[] CCproperty = null;

    public CClabel(File inFile) throws Exception
    {
        //In File
        Scanner scinFile = new Scanner(inFile);
        numRows = 0;
        if(scinFile.hasNextInt()) numRows = scinFile.nextInt();
        numCols = 0;
        if(scinFile.hasNextInt()) numCols = scinFile.nextInt();
        minVal = 0;
        if(scinFile.hasNextInt()) minVal = scinFile.nextInt();
        maxVal = 0;
        if(scinFile.hasNextInt()) maxVal = scinFile.nextInt();
        scinFile.close();
        System.out.println("[CClabel] inFile first line data loaded");

        //Init variable
        zeroFramedAry = new int[numRows + 2][numCols + 2];
        NonZeroNeighborAry = new int[5];

        EQAry = new int[(numRows * numCols) / 4];
        newLabel = 0;
        System.out.println("[CClabel] Allocated variables");
    }

    public void zero2D(int[][] arr)
    {
        for(int i = 0; i < arr.length; i++)
        {
            for(int j = 0; j < arr[i].length; j++)
            {
                arr[i][j] = 0;
            }
        }
    }

    public void minus1D(int[][] arr)
    {
        for(int i = 0; i < arr.length; i++)
        {
            for(int j = 0; j < arr[j].length; j++)
            {
                arr[i][j] = -1;
            }
        }
    }

    public void loadImage(File inFile, int[][] arr) throws Exception
    {
        Scanner scinFile = new Scanner(inFile);
        //Skip the first line
        scinFile.nextLine();

        //Load image
        //Start from 1,1
        for(int i = 1; i < arr.length - 1; i++)
        {
            String strData[] = (scinFile.nextLine()).split("\\s+");
            
            for(int j = 0; j < strData.length; j++)
            {
                arr[i][j + 1] = Integer.parseInt(strData[j]);
            }
        }
        scinFile.close();

        System.out.println("[CClabel] Loaded image data");
    }

    void imgReformat(int[][] inAry, FileWriter OutImg, String strCaption)
    {
        try
        {
            //Grab the min and max value
            int numMin = Integer.MAX_VALUE;
            int numMax = 0;
            for(int i = 1; i < inAry.length - 1; i++)
            {
                for(int j = 1; j < inAry[i].length - 1; j++)
                {
                    if(inAry[i][j] > numMax)
                    {
                        numMax = inAry[i][j];
                    }
                    if(inAry[i][j] < numMin)
                    {
                        numMin = inAry[i][j];
                    }
                }
            }

            //Output them
            OutImg.write(strCaption + "\n");
            OutImg.write(String.format("%d %d %d %d\n", numRows, numCols, numMin, numMax));
            String strMax = Integer.toString(numMax);
            int nWidth = strMax.length();
            int r = 1, c = 1;

            while(r <= numRows)
            {
                while(c <= numCols)
                {
                    String str;
                    if(inAry[r][c] > 0)
                    {
                        str = Integer.toString(inAry[r][c]);
                    }
                    else
                    {
                        str = ".";
                    }
                    
                    OutImg.write(str);
                    int WW = str.length();
                    OutImg.write(' ');
                    while(WW < nWidth)
                    {
                        OutImg.write(' ');
                        WW++;
                    }
                    c++;
                }
                r++;
                c = 1;
                OutImg.write("\n");
            }

            OutImg.write("\n\n");
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }

    }

    int findMin(int... values)
    {
        int min = EQAry.length;
        for(int each : values)
        {
            if((each < min) && (each != 0))
            {
                min = each;
            }
        }
        return min;
    }

    int findMax(int... values)
    {
        int max = 0;
        for(int each : values)
        {
            if((each > max) && (each != 0))
            {
                max = each;
            }
        }
        return max;
    }

    public void connected8Pass1(int[][] ary, int[] EQAry)
    {
        for(int i = 1; i < ary.length - 1; i++)
        {
            for(int j = 1; j < ary[i].length - 1; j++)
            {
                if(ary[i][j] == 0)
                {
                    continue;
                }

                int a = ary[i - 1][j - 1];
                int b = ary[i - 1][j];
                int c = ary[i - 1][j + 1];
                int d = ary[i][j - 1];

                int min = findMin(a, b, c, d);
                int max = findMax(a, b, c, d);
                
                //Case 1
                if((a == 0) && (b == 0) && (c == 0) && (d == 0))
                {
                    newLabel++;
                    ary[i][j] = newLabel;
                    EQAry[newLabel] = newLabel;
                }
                //Case 2 if they the same
                else if(a == b && b == c && c == d)
                {
                    ary[i][j] = a;
                }
                //Case 3
                else
                {
                    ary[i][j] = min;
                    EQAry[max] = min;
                }
            }
        }
        System.out.println("[CClabel] 8-Connected pass 1 completed");
    }

    public void connected8Pass2(int[][] ary, int[] EQAry)
    {
        for(int i = ary.length - 2; i > 0; i--)
        {
            for(int j = ary[i].length - 2; j > 0; j--)
            {
                if(ary[i][j] == 0)
                {
                    continue;
                }

                int e = ary[i][j + 1];
                int f = ary[i + 1][j - 1];
                int g = ary[i + 1][j];
                int h = ary[i + 1][j + 1];
                
                //Case 1
                if((e == 0) && (f == 0) && (g == 0) && (h == 0))
                {
                    //Keep it label do nothing
                }
                //Case 2
                else if(e == f && f == g && g == h)
                {
                    //Keep it label do nothing
                }
                //Case 3
                else if(e != f)
                {
                    int min = findMin(e, f, g, h);
                    if(ary[i][j] > min)
                    {
                        EQAry[ary[i][j]] = min;
                        ary[i][j] = min;
                        continue;
                    }
                }
                
                //Step 3 only update P(i, j) not not update in step 2 (case 1, case 2)
                ary[i][j] = EQAry[ary[i][j]];
            }
        }
        System.out.println("[CClabel] 8-Connected pass 2 completed");
    }

    public void connected4Pass1(int[][] ary, int[] EQAry)
    {
        for(int i = 1; i < ary.length - 1; i++)
        {
            for(int j = 1; j < ary[i].length - 1; j++)
            {
                if(ary[i][j] == 0)
                {
                    continue;
                }

                int b = ary[i - 1][j];
                int d = ary[i][j - 1];

                int min = findMin(b, d);
                int max = findMax(b, d);
                
                //Case 1
                if((b == 0) && (d == 0))
                {
                    newLabel++;
                    ary[i][j] = newLabel;
                    EQAry[newLabel] = newLabel;
                }
                //Case 2 if they the same
                else if(b == d)
                {
                    ary[i][j] = b;
                }
                //Case 2 if they are not the same then we will the number that exist
                else if(b == 0 || d == 0)
                {
                    ary[i][j] = min;
                }
                //Case 3
                else
                {
                    ary[i][j] = min;
                    EQAry[max] = min;
                }
            }
        }
        System.out.println("[CClabel] 4-Connected pass 1 completed");
    }

    public void connected4Pass2(int[][] ary, int[] EQAry)
    {
        for(int i = ary.length - 2; i > 0; i--)
        {
            for(int j = ary[i].length - 2; j > 0; j--)
            {
                if(ary[i][j] == 0)
                {
                    continue;
                }

                int e = ary[i][j + 1];
                int g = ary[i + 1][j];
                
                //Case 1
                if((e == 0) && (g == 0))
                {
                    //Keep it label do nothing
                }
                //Case 2
                else if(e == g)
                {
                    //Keep it label do nothing
                }
                //Case 3
                else if(e != g)
                {
                    int min = findMin(e, g);
                    if(ary[i][j] > min)
                    {
                        EQAry[ary[i][j]] = min;
                        ary[i][j] = min;
                        continue;
                    }
                }
                
                //Step 3 only update P(i, j) not not update in step 2
                ary[i][j] = EQAry[ary[i][j]];
            }
        }
        System.out.println("[CClabel] 4-Connected pass 2 completed");
    }

    public void connectPass3(int[][] ary, int[] EQAry, Property[] CCproperty)
    {
        for(int i = 1; i <= trueNumCC; i++)
        {
            CCproperty[i].label = i;
            CCproperty[i].numPixels = 0;
            CCproperty[i].minR = numRows;
            CCproperty[i].maxR = 0;
            CCproperty[i].minC = numCols;
            CCproperty[i].maxC = 0;
        }

        for(int i = 0; i < ary.length; i++)
        {
            for(int j = 0; j < ary[i].length; j++)
            {
                if(ary[i][j] == 0)
                {
                    continue;
                }

                ary[i][j] = EQAry[ary[i][j]];
                int k = ary[i][j];
                CCproperty[k].numPixels++;
                if(i < CCproperty[k].minR)
                {
                    CCproperty[k].minR = i;
                }
                if(i > CCproperty[k].maxR)
                {
                    CCproperty[k].maxR = i;
                }
                if(j < CCproperty[k].minC)
                {
                    CCproperty[k].minC = j;
                }
                if(j > CCproperty[k].maxC)
                {
                    CCproperty[k].maxC = j;
                }
            }
        }
        System.out.println("[CClabel] Pass 3 completed");
    }

    public void drawBoxes(int[][] ary, Property[] CCproperty)
    {
        for(int i = 1; i <= trueNumCC; i++)
        {
            int minRow = CCproperty[i].minR;
            int minCol = CCproperty[i].minC;
            int maxRow = CCproperty[i].maxR;
            int maxCol = CCproperty[i].maxC;
            int label = CCproperty[i].label;

            for(int r = minRow; r <= maxRow; r++)
            {
                for(int c = minCol; c <= maxCol; c++)
                {
                    ary[minRow][c] = label;
                    ary[maxRow][c] = label;
                }
                ary[r][minCol] = label;
                ary[r][maxCol] = label;
            }
        }
        System.out.println("[CClabel] Draw boxes completed");
    }

    public int manageEQAry(int[] EQAry)
    {
        int readLabel = 0;
        for(int i = 1; i <= newLabel; i++)
        {
            if(i != EQAry[i])
            {
                EQAry[i] = EQAry[EQAry[i]];
            }
            else
            {
                readLabel++;
                EQAry[i] = readLabel;
            }
        }

        System.out.println("[CClabel] EQTable Management completed");

        return readLabel;
    }

    public void printCCproperty(FileWriter OutFile)
    {
        try
        {
            OutFile.write(String.format("%d %d %d %d\n", numRows, numCols, newMin, newMax));
            OutFile.write(String.format("%d\n", trueNumCC));
            for(int i = 1; i < CCproperty.length; i++)
            {
                OutFile.write(CCproperty[i].label + "\n");
                OutFile.write(CCproperty[i].numPixels + "\n");
                OutFile.write((CCproperty[i].minR - 1) + " " + (CCproperty[i].minC - 1) + "\n");
                OutFile.write((CCproperty[i].maxR - 1) + " " + (CCproperty[i].maxC - 1) + "\n");
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }

        System.out.println("[CClabel] Output CCProperty completed");
    }

    public void printEQAry(int[] EQAry, int newLabel, FileWriter OutFile, String strCaption)
    {
        try
        {
            OutFile.write(strCaption + "\n");
            for (int i = 1; i <= newLabel; i++)
            {
                OutFile.write(EQAry[i] + " ");
            }
            OutFile.write("\n\n\n");
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }

    public void printImg(FileWriter OutFile)
    {
        try
        {
            //Output them
            OutFile.write(String.format("%d %d %d %d\n", numRows, numCols, newMin, newMax));
            String strMax = Integer.toString(newMax);
            int nWidth = strMax.length();
            int r = 1, c = 1;

            while(r <= numRows)
            {
                while(c <= numCols)
                {
                    String str;
                    if(zeroFramedAry[r][c] > 0)
                    {
                        str = Integer.toString(zeroFramedAry[r][c]);
                    }
                    else
                    {
                        str = ".";
                    }
                    
                    OutFile.write(str);
                    int WW = str.length();
                    OutFile.write(' ');
                    while(WW < nWidth)
                    {
                        OutFile.write(' ');
                        WW++;
                    }
                    c++;
                }
                r++;
                c = 1;
                OutFile.write("\n");
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }

        System.out.println("[CClabel] Output Final Image completed");
    }
}
