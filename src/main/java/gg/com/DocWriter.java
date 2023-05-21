package gg.com;

import java.io.Writer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class DocWriter {

    static public void write(String path, String data) {
        try {
            Path dataPath = Paths.get("config", path);
            Writer writer = Files.newBufferedWriter(dataPath);
            writer.write(data);
            writer.close();
        } catch (Exception e) {
            System.out.println("File not found.");
        }
    }
}