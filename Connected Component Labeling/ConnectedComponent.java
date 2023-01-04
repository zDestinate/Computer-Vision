import java.io.File;
import java.io.FileWriter;

public class ConnectedComponent
{
    public static void main(String[] args) throws Exception
    {
        if(args.length != 5)
        {
            throw new IllegalArgumentException("[ERROR] Invalid arguments. Require 5 arguments");
        }

        File inFile = new File(args[0]);
        if(!inFile.exists())
        {
            throw new IllegalArgumentException("[ERROR] Invalid input file");
        }
        System.out.println("[Info] inFile loaded");

        //Which connectness 8 or 4
        int Connectness = Integer.parseInt(args[1]);

        //Output
        FileWriter RFprettyPrintFile = new FileWriter(args[2]);
        FileWriter labelFile = new FileWriter(args[3]);
        FileWriter propertyFile = new FileWriter(args[4]);
        System.out.println("[Info] Output files loaded");
        

        //Connected Component class
        CClabel CC = new CClabel(inFile);
        CC.zero2D(CC.zeroFramedAry);
        CC.loadImage(inFile, CC.zeroFramedAry);

        if(Connectness == 4)
        {
            CC.connected4Pass1(CC.zeroFramedAry, CC.EQAry);
            CC.imgReformat(CC.zeroFramedAry, RFprettyPrintFile, "4-Connected Pass 1");
            CC.printEQAry(CC.EQAry, CC.newLabel, RFprettyPrintFile, "4-Connected Pass 1 EQTable");
            CC.connected4Pass2(CC.zeroFramedAry, CC.EQAry);
            CC.imgReformat(CC.zeroFramedAry, RFprettyPrintFile, "4-Connected Pass 2");
            CC.printEQAry(CC.EQAry, CC.newLabel, RFprettyPrintFile, "4-Connected Pass 2 EQTable");
        }
        else
        {
            CC.connected8Pass1(CC.zeroFramedAry, CC.EQAry);
            CC.imgReformat(CC.zeroFramedAry, RFprettyPrintFile, "8-Connected Pass 1");
            CC.printEQAry(CC.EQAry, CC.newLabel, RFprettyPrintFile, "8-Connected Pass 1 EQTable");
            CC.connected8Pass2(CC.zeroFramedAry, CC.EQAry);
            CC.imgReformat(CC.zeroFramedAry, RFprettyPrintFile, "8-Connected Pass 2");
            CC.printEQAry(CC.EQAry, CC.newLabel, RFprettyPrintFile, "8-Connected Pass 2 EQTable");
        }

        //Step 6
        CC.trueNumCC = CC.manageEQAry(CC.EQAry);
        CC.printEQAry(CC.EQAry, CC.newLabel, RFprettyPrintFile, "EQTable after EQTable Management");
        CC.newMin = 0;
        CC.newMax = CC.trueNumCC;
        CC.CCproperty = new Property[CC.trueNumCC + 1];
        for(int i = 0; i < CC.CCproperty.length; i++)
        {
            CC.CCproperty[i] = new Property();
        }

        //Step 7 - 9
        CC.connectPass3(CC.zeroFramedAry, CC.EQAry, CC.CCproperty);
        CC.imgReformat(CC.zeroFramedAry, RFprettyPrintFile, "Result of Pass 3");
        CC.printEQAry(CC.EQAry, CC.newLabel, RFprettyPrintFile, "Pass 3 EQTable");

        CC.printImg(labelFile);
        CC.printCCproperty(propertyFile);
        CC.drawBoxes(CC.zeroFramedAry, CC.CCproperty);
        CC.imgReformat(CC.zeroFramedAry, RFprettyPrintFile, "Bounding Box");
        RFprettyPrintFile.write("Number of Connected Components: " + CC.trueNumCC);

        RFprettyPrintFile.close();
        labelFile.close();
        propertyFile.close();
    }
}
