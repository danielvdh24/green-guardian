package gg.com;

import javafx.beans.Observable;

public class TableEntry /*implements Observable*/ /*TO BE IMPLEMENTED*/ {
    
    final Integer ID;
    String name;
    int CT;
    int CH;
    int CL;
    int OT;
    int OH;
    int OL;
    String status;

    TableEntry(Integer ID, String name,int CT,int CH,int CL,int OT,int OH,int OL,String status)
    {
        this.name=name;
        this.ID=ID;
        this.CH=CH;
        this.CT=CT;
        this.CL=CL;
        this.OH=OH;
        this.OT=OT;
        this.OL=OL;
        this.status=status;
    }

    Integer getID()
    {
        return this.ID;
    }
    
}
