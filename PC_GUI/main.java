package PC_GUI;

class Main
{
    public static void main(String[] args) 
    {   
        Reader reader = new Reader("PC_GUI\\Test_Data\\test1.txt");
        int N_Plants = reader.nextInt();
        for(int i=0;i<N_Plants;i++)
        {
            for(int j=0;j<7*48;j++)
            {
                System.out.println(reader.nextInt()+" "+reader.nextInt());
            }
            System.out.println();
        }
    }
}