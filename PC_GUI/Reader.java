package PC_GUI;

import java.util.Scanner;
import java.io.File;

public class Reader {

    File file;
    Scanner scanner;

    Reader(String path)
    {
        try
        {
            file = new File(path);
            System.out.println(file.exists());
            scanner = new Scanner(file);
        }
        catch(Exception e)
        {
            System.out.println("No file found.");
        }
    }

    public int nextInt()
    {
        try
        {
            return scanner.nextInt();
        }
        catch(Exception e)
        {
            System.out.println("Input invalid.");
            return -1;
        }
    }

    
}
