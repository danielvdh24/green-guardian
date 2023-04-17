package PC_GUI;

class Main
{
    public static void main(String[] args) 
    {   
        Reader reader = new Reader("Test_data\\test1.txt");
        for(int i=0;i<30;i++)System.out.println(reader.nextInt());
    }
}