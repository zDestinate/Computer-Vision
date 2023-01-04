import java.io.File;
import java.io.FileWriter;
import java.util.Scanner;

public class imageProcessing
{
    protected int numRows, numCols, minVal, maxVal,
        maskRows, maskCols, maskMin, maskMax,
        newMin, newMax, thrVal,
        mirrorFramedAry[][], avgAry[][], medianAry[][], GaussAry[][], thrAry[][], mask2DAry[][],
        neighbor1DAry[], mask1DAry[];

    imageProcessing(File inFile, File maskFile, int thrVal) throws Exception
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
        System.out.println("[INFO] inFile first line data loaded");

        //Mask File
        Scanner scmaskFile = new Scanner(maskFile);
        maskRows = 0;
        if(scmaskFile.hasNextInt()) maskRows = scmaskFile.nextInt();
        maskCols = 0;
        if(scmaskFile.hasNextInt()) maskCols = scmaskFile.nextInt();
        maskMin = 0;
        if(scmaskFile.hasNextInt()) maskMin = scmaskFile.nextInt();
        maskMax = 0;
        if(scmaskFile.hasNextInt()) maskMax = scmaskFile.nextInt();
        scmaskFile.close();
        System.out.println("[INFO] maskFile first line data loaded");
        
        //Threshold value
        if(thrVal > maxVal || thrVal < minVal)
        {
            throw new Exception("[ERROR] Threshold value exceed maxVal or less than minVal");
        }
        this.thrVal = thrVal;
        System.out.println("[INFO] Threshold value set");

        //Misc
        //Allocate memory
        int extraRows = maskRows / 2;
        int extraCols = maskCols / 2;
        mirrorFramedAry = new int[numRows + (extraRows * 2)][numCols + (extraCols * 2)];
        thrAry = new int[numRows + (extraRows * 2)][numCols + (extraCols * 2)];
        avgAry = new int[numRows + (extraRows * 2)][numCols + (extraCols * 2)];
        medianAry = new int[numRows + (extraRows * 2)][numCols + (extraCols * 2)];
        GaussAry = new int[numRows + (extraRows * 2)][numCols + (extraCols * 2)];
    }

    void threshold(int[][] ary1, int[][] ary2)
    {
        newMin = 0;
        newMax = 1;

        int extraRows = maskRows / 2;
        int extraCols = maskCols / 2;

        for(int i = extraRows; i <= numRows; i++)
        {
            for(int j = extraCols; j <= numCols; j++)
            {
                ary2[i][j] = (ary1[i][j] >= thrVal) ? 1 : 0;
            }
        }
    }

    void imgReformat(int[][] inAry, int newMin, int newMax, FileWriter OutImg)
    {
        try
        {
            OutImg.write(String.format("%d %d %d %d\n", numRows, numCols, newMin, newMax));
            String strMax = Integer.toString(newMax);
            int nWidth = strMax.length();
            int r = 1, c = 1;

            while(r <= numRows)
            {
                while(c <= numCols)
                {
                    String str = Integer.toString(inAry[r][c]);
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
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
        
    }

    void mirrorFraming()
    {
        int extraRows = maskRows / 2;
        int extraCols = maskCols / 2;

        //Mirroring
        int nRow = extraRows * 2;
        int nCol = extraCols * 2;
        //Rows
        for(int i = 0; i < extraRows; i++)
        {
            //Top extra rows
            for(int j = 0; j < mirrorFramedAry[i].length; j++)
            {
                if((j < extraCols) || (j >= (mirrorFramedAry[i].length - 1 - extraCols)))
                {
                    mirrorFramedAry[i][j] = mirrorFramedAry[nRow][nCol--];
                    continue;
                }
                
                mirrorFramedAry[i][j] = mirrorFramedAry[nRow][nCol++];
            }

            //Bottom extra rows
            nCol = extraCols * 2;
            for(int j = 0; j < mirrorFramedAry[i].length; j++)
            {
                if((j < extraCols) || (j >= (mirrorFramedAry[i].length - 1 - extraCols)))
                {
                    mirrorFramedAry[mirrorFramedAry.length - 1 - i][j] = mirrorFramedAry[mirrorFramedAry.length - 1 - nRow][nCol--];
                    continue;
                }
                
                mirrorFramedAry[mirrorFramedAry.length - 1 - i][j] = mirrorFramedAry[mirrorFramedAry.length - 1 - nRow][nCol++];
            }
            
            nRow--;
        }

        //Left extra columns
        nCol = extraCols * 2;
        for(int i = 0; i < extraCols; i++)
        {
            for(int j = extraRows; j < mirrorFramedAry.length - extraRows; j++)
            {
                mirrorFramedAry[j][i] = mirrorFramedAry[j][nCol];
            }

            nCol--;
        }

        //Right extra columns
        nCol = extraCols * 2;
        for(int i = 0; i < extraCols; i++)
        {
            for(int j = extraRows; j < mirrorFramedAry.length - extraRows; j++)
            {
                mirrorFramedAry[j][mirrorFramedAry[j].length - 1 + i] = mirrorFramedAry[j][mirrorFramedAry[j].length - 1 - nCol];
            }

            nCol++;
        }

        System.out.println("[INFO] Mirror Framed");
    }

    void loadImage(File inFile)
    {
        try
        {
            int extraRows = maskRows / 2;
            int extraCols = maskCols / 2;

            Scanner scinFile = new Scanner(inFile);
            //Skip the first line
            scinFile.nextLine();
    
            //Load image
            for(int i = extraRows; i < mirrorFramedAry.length - extraRows; i++)
            {
                String strData[] = (scinFile.nextLine()).split(" ");
                for(int j = 0; j < strData.length; j++)
                {
                    mirrorFramedAry[i][extraCols + j] = Integer.parseInt(strData[j]);
                }
            }
            scinFile.close();

            System.out.println("[INFO] Loaded image data");
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }

    void loadMask(File maskFile)
    {
        try
        {
            mask2DAry = new int[maskRows][maskCols];

            Scanner scmaskFile = new Scanner(maskFile);
            scmaskFile.nextLine();
            for(int i = 0; i < maskRows; i++)
            {
                String strData[] = (scmaskFile.nextLine()).split(" ");
                for(int j = 0; j < maskCols; j++)
                {
                    mask2DAry[i][j] = Integer.parseInt(strData[j]);
                }
            }
            scmaskFile.close();

            System.out.println("[INFO] Loaded mask file data");
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }

    void loadMask1DAry()
    {
        mask1DAry = new int[maskRows * maskCols];

        int n = 0;
        for(int i = 0; i < maskRows; i++)
        {
            for(int j = 0; j < maskCols; j++)
            {
                mask1DAry[n++] = mask2DAry[i][j];
            }
        }

        System.out.println("[INFO] Loaded mask 1D array");
    }

    void loadNeightboard1DAry(int i, int j, int[][] neighborAry)
    {
        neighbor1DAry = new int[maskRows * maskCols];
        int n = 0;

        int extraRows = maskRows / 2;
        int extraCols = maskCols / 2;

        for(int x = (extraRows * -1); x <= extraRows; x++)
        {
            for(int y = (extraCols * -1); y <= extraCols; y++)
            {
                neighbor1DAry[n++] = neighborAry[i + x][j + y];
            }
        }
    }

    void sort(int[] neighborAry)
    {
        int n = 0;
        while(n < (neighborAry.length - 1))
        {
            if(neighborAry[n] > neighborAry[n + 1])
            {
                int temp = neighborAry[n];
                neighborAry[n] = neighborAry[n + 1];
                neighborAry[n + 1] = temp;
                n = 0;
            }
            else
            {
                n++;
            }
        }
    }

    void computeAvg()
    {
        newMin = 9999;
        newMax = 0;

        int extraRows = maskRows / 2;
        int extraCols = maskCols / 2;
        for(int i = extraRows; i <= numRows; i++)
        {
            for(int j = extraCols; j <= numCols; j++)
            {
                int sum = mirrorFramedAry[i - 1][j - 1] + mirrorFramedAry[i][j - 1] + mirrorFramedAry[i + 1][j - 1] + mirrorFramedAry[i - 1][j] + mirrorFramedAry[i][j] + mirrorFramedAry[i + 1][j] + mirrorFramedAry[i - 1][j + 1] + mirrorFramedAry[i][j + 1] + mirrorFramedAry[i + 1][j + 1];
                avgAry[i][j] = sum / (maskCols * maskRows);
                if(newMin > avgAry[i][j])
                {
                    newMin = avgAry[i][j];
                }
                if(newMax < avgAry[i][j])
                {
                    newMax = avgAry[i][j];
                }
            }
        }
    }

    void computeMedian()
    {
        newMin = 9999;
        newMax = 0;
        int extraRows = maskRows / 2;
        int extraCols = maskCols / 2;   

        for(int i = extraRows; i <= numRows; i++)
        {
            for(int j = extraCols; j <= numCols; j++)
            {
                loadNeightboard1DAry(i, j, mirrorFramedAry);
                sort(neighbor1DAry);
                medianAry[i][j] = neighbor1DAry[neighbor1DAry.length / 2];
                if(newMin > medianAry[i][j])
                {
                    newMin = medianAry[i][j];
                }
                if(newMax < medianAry[i][j])
                {
                    newMax = medianAry[i][j];
                }
            }
        }
    }

    void computerGauss()
    {
        newMin = 9999;
        newMax = 0;
        int extraRows = maskRows / 2;
        int extraCols = maskCols / 2;   

        int nMaskSum = 0;
        for(int i = 0; i < mask1DAry.length; i++)
        {
            nMaskSum += mask1DAry[i];
        }

        for(int i = extraRows; i <= numRows; i++)
        {
            for(int j = extraCols; j <= numCols; j++)
            {
                loadNeightboard1DAry(i, j, mirrorFramedAry);
                GaussAry[i][j] = convolution(neighbor1DAry, mask1DAry) / nMaskSum;
                if(newMin > GaussAry[i][j])
                {
                    newMin = GaussAry[i][j];
                }
                if(newMax < GaussAry[i][j])
                {
                    newMax = GaussAry[i][j];
                }
            }
        }
    }

    int convolution(int neighbor1D[], int mask1D[])
    {
        int result = 0;
        for(int i = 0; i < 9; i++)
        {
            result += neighbor1D[i] * mask1D[i];
        }
        return result;
    }
}
