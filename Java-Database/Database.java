import java.util.ArrayList;
import java.util.Scanner;
import java.io.PrintWriter;
import java.io.File;

class Database {
    String name;
    ArrayList<Table> tables;

    private void construct(String title) {
        tables = new ArrayList<Table>();
        name = title;
    }

    boolean tableExists(String string) {
        for(Table table : tables) {
            if(table.name.equals(string)) return true;
        }
        return false;
    }
    
    // Write the metadata to the file in the database directory.
    private void writeCatalog() {
        File file = new File("./" + name + "/" + "catalog-metadata" + ".txt");
        PrintWriter writer = null;
        try {
            writer = new PrintWriter(file, "UTF-8");
        } catch(Exception e) { throw new Error(e); }

        for(Table table : tables) {
            writer.print(table.name + "/");
        }
        writer.close();
    }

    // Add the given table to the database.
    boolean add(Table table) {
        if(tableExists(table.name)) {
            System.err.println("Table exists: " + table.name);
            return false;
        }
        tables.add(table);
        writeCatalog();
        return true;
    }

    // Returns the table with the given name.
    Table get(String name) {
        for(Table table : tables) {
            if(table.name.equals(name)) return table;
        }
        return null;
    }

    // Returns an ArrayList of table names in the database.
    ArrayList<String> list() {
        ArrayList<String> names = new ArrayList<String>();
        for(Table table : tables) {
            names.add(table.name);
        }
        return names;
    }

    // Load the table from the file in the database directory.
    boolean load() {
        File file = new File("./" + name + "/" + "catalog-metadata" + ".txt");
        try {
            Scanner scan = new Scanner(file);
            String[] tableList = scan.nextLine().split("/");
            for(String title : tableList) {
                Table table = new Table();
                if(!table.load(this.name, title) || !add(table)) {
                    System.err.println("Error with table " + table.name);
                    return false;
                }
            }
        }
        catch(Exception e) { 
            System.err.println("This database has no tables.");
        }
        return true;
    }

    void create(String title) {
        construct(title);
        new File("./"+title).mkdir(); // If dir exists this will do nothing
    }
}

