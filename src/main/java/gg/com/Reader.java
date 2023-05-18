package gg.com;

import java.io.BufferedReader;
import java.io.File;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Reader {

    File file;
    BufferedReader reader;

    Reader(String path)
    {
        try {
            Path dataPath = Paths.get("config", path);
            reader = Files.newBufferedReader(dataPath);
        } catch (Exception e){
            e.printStackTrace();
        }
    }

    public int nextInt()
    {
        try
        {
            char c = (char)reader.read();
            String result = "";
            while(!(c>='0'&&c<='9'))
            {
                c = (char)reader.read();
            }
            while(c>='0'&&c<='9')
            {
                result+=c;
                
                c = (char)reader.read();
            }
            if(result=="")return -1;
            return Integer.parseInt(result);
        }
        catch(Exception e)
        {
            System.out.println("Input invalid.");
            return -1;
        }
    }

    public String nextLine()
    {
        try
        {
            return reader.readLine();
        }
        catch(Exception e)
        {
            System.out.println("Input invalid.");
            return null;
        }
    }
    
}
