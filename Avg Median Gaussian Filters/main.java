import java.io.File;
import java.io.FileWriter;

public class AvgMedianGausFilters
{
    public static void main(String[] args)
    {
        try
        {
            if(args.length != 10)
            {
                throw new IllegalArgumentException("[ERROR] Invalid arguments");
            }

            File inFile = new File(args[0]);
            if(!inFile.exists())
            {
                throw new IllegalArgumentException("[ERROR] Invalid input file");
            }

            File maskFile = new File(args[1]);
            if(!maskFile.exists())
            {
                throw new IllegalArgumentException("[ERROR] Invalid mask file");
            }

            //Output
            FileWriter inputImg = new FileWriter(args[3]);
            FileWriter AvgOut = new FileWriter(args[4]);
            FileWriter AvgThreshold = new FileWriter(args[5]);
            FileWriter MedianOut = new FileWriter(args[6]);
            FileWriter MedianThresHold = new FileWriter(args[7]);
            FileWriter GaussOut = new FileWriter(args[8]);
            FileWriter GaussThreshold = new FileWriter(args[9]);

            
            //imageProcessing
            imageProcessing imgProcessing = new imageProcessing(inFile, maskFile, Integer.parseInt(args[2]));
            imgProcessing.loadMask(maskFile);
            imgProcessing.loadMask1DAry();
            imgProcessing.loadImage(inFile);

            imgProcessing.mirrorFraming();
            imgProcessing.imgReformat(imgProcessing.mirrorFramedAry, imgProcessing.minVal, imgProcessing.maxVal, inputImg);
            System.out.println("[OUTPUT] Original Image format");

            imgProcessing.computeAvg();
            imgProcessing.imgReformat(imgProcessing.avgAry, imgProcessing.newMin, imgProcessing.newMax, AvgOut);
            System.out.println("[OUTPUT] Average filter");
            imgProcessing.threshold(imgProcessing.avgAry, imgProcessing.thrAry);
            imgProcessing.imgReformat(imgProcessing.thrAry, 0, 1, AvgThreshold);
            System.out.println("[OUTPUT] Average filter with threshold");

            imgProcessing.computeMedian();
            imgProcessing.imgReformat(imgProcessing.medianAry, imgProcessing.newMin, imgProcessing.newMax, MedianOut);
            System.out.println("[OUTPUT] Median filter");
            imgProcessing.threshold(imgProcessing.medianAry, imgProcessing.thrAry);
            imgProcessing.imgReformat(imgProcessing.thrAry, 0, 1, MedianThresHold);
            System.out.println("[OUTPUT] Median filter with threshold");

            imgProcessing.computerGauss();
            imgProcessing.imgReformat(imgProcessing.GaussAry, imgProcessing.newMin, imgProcessing.newMax, GaussOut);
            System.out.println("[OUTPUT] Gaussian filter");
            imgProcessing.threshold(imgProcessing.GaussAry, imgProcessing.thrAry);
            imgProcessing.imgReformat(imgProcessing.thrAry, 0, 1, GaussThreshold);
            System.out.println("[OUTPUT] Gaussian filter with threshold");
            

            //Close files
            inputImg.close();
            AvgOut.close();
            AvgThreshold.close();
            MedianOut.close();
            MedianThresHold.close();
            GaussOut.close();
            GaussThreshold.close();
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }
}
