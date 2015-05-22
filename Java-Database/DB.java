import java.util.Arrays;
import java.util.ArrayList;
import java.util.Scanner;
import java.io.File;
import java.util.Stack;

// This class is an interface for interacting with databases.
class DB {
    Display display;
    Database database;
    Stack<Table> tables;
    Table current;

    private class Display {

        void printColumnNames() {
            for(int i=0; i<current.size(); i++) {
                System.out.print("|");
                String name = current.columns.get(i).name;
                System.out.print(" " + name + " ");
                printSpaces(current.getCol(name).width, name);
            }
        }

        void printBoundary() {
            for(int j=0; j<current.size(); j++) {
            System.out.print("|");
                for(int i=0; i<current.columns.get(j).width+2; i++) {
                    System.out.print("-");
                }
            }
        }

        void printHeader() {
            printColumnNames();
            System.out.println("|");
            printBoundary();
            System.out.println("|");
        }

        void printRecords() {
            for(Record record : current.records) {
                for(int i=0; i<current.size(); i++) {
                    System.out.print("|");
                    String entry = record.get(i);
                    boolean r = current.RType(i); // right-alignment
                    if(r) printSpaces(current.columns.get(i).width, entry);
                    System.out.print(" " + entry + " ");
                    if(!r) printSpaces(current.columns.get(i).width, entry);
                }
                System.out.println("|");
            }
        }

        void printCurrent() {
            System.out.println("\n" + current.name);
            printHeader();
            printRecords();
            System.out.println();
        }

        void printSpaces(int width, String entry) {
            int diff = width - entry.length();
            for(int i=0; i<diff; i++) {
                System.out.print(" ");
            }
        }

        void list(ArrayList<String> list) {
            for(String string : list) {
                System.out.print(string + " | ");
            }
            System.out.println();
        }
    }

    private void construct() {
        display = new Display();
        database = new Database();
        tables = new Stack<Table>();
        current = null;
    }

    private Table searchStack(String requested) {
        for(Table table : tables) {
            if(table.name.equals(requested)) return table;
        }
        return null;
    }

    // Load or create a table under the given name.
    private boolean pushTable(String name) {
        Table table = new Table();
        Table found = searchStack(name);
        if(found!=null) table = found;
        else {
            table.create(name);
            if(!database.tableExists(name)) {
                if(!confirmCreation(name)) return false;
            }
            else {
                table.load(database.name, name);
            }
        }
        if(table == null) {
            System.err.println("Table " + name + " doesn't exist.");
            return false;
        }
        tables.push(table);
        current = table;
        return true;
    }

    // Push the given table.
    boolean cmdTable(ArrayList<String> words) {
        if(words.size() != 2) {
            System.err.println("table TABLE");
            return false;
        }
        return pushTable(words.get(1));
    }

    boolean nameTable(Table table) {
        System.err.println("Name of this table?");
        String string = acceptLine();
        if(string.indexOf("/")!=-1 || string.indexOf("\n")!=-1 
             || string.indexOf("-")!=-1) {
            System.err.println("Invalid entry; contains forbidden characters.");
            return false;
        }
        table.name = string;
        return true;
    }

    // Create a new table with the given columns and types
    boolean cmdNew(ArrayList<String> words) {
        if(words.size() < 2 || (words.size()%2) == 0) {
            System.err.println("new COLUMN_A TYPE_A COLUMN_B TYPE_B ...");
            return false;
        }
        Table table = new Table();
        table.create("tmp");
        for(int i=1; i<words.size(); i+=2) {
            if(!table.add(words.get(i), words.get(i+1), "na")) return false;
        }
        if(!nameTable(table)) return false;
        current = table;
        return true;
    }

    boolean newCurrent(ArrayList<String> words) {
        System.err.println("Table does not exist.");
        System.err.println("Save current table under this name? y/n");
        if(acceptYN()) {
            if(words.get(1).indexOf("/")!=-1 
                 || words.get(1).indexOf("\n")!=-1 
                   || words.get(1).indexOf("-")!=-1) {
            System.err.println("Entry contains forbidden characters.");
            return false;
            }
            current.save(database.name, words.get(1));
            current.name = words.get(1);
            database.add(current);
            return true;
        }
        return false;
    }

    boolean overwriteExisting(String tableName) {
        System.err.println("Overwrite " + tableName + "? y/n");
        if(!acceptYN()) return false;
        current.save(database.name, tableName);
        return true;
    }

    // Save the current table under the given name.
    boolean cmdSave(ArrayList<String> words) {
        if(words.size() < 2) {
            System.err.println("save TABLE");
            return false;
        }
        Table table = database.get(words.get(1));
        if(table == null) return newCurrent(words);
        else return overwriteExisting(table.name);
    }

    // Trim the rows to match the columns requested by cmdColumns.
    void cutRows() {
        Table top = tables.peek();
        for(Record record : top.records) {
            Record cut = new Record();
            cut.create();
            for(int i=0; i<current.columns.size(); i++) {
                cut.add(record.get(top.columns.indexOf(
                    top.getCol(current.columns.get(i).name))));
            }
            current.add(cut); 
        }       
    }

    // Take the given columns of the current table.
    boolean cmdColumns(ArrayList<String> words) {
        if(words.size() < 2) {
            System.err.println("columns COLUMN_A COLUMN_B ...");
            return false;
        }
        String name = current.name;
        current = new Table();
        current.create(name);
        for(int i=1; i<words.size(); i++) {
            if(tables.peek().getCol(words.get(i))==null) {
                System.err.println("Column does not exist: " + words.get(i));
                return false;
            }
            if(!current.add(tables.peek().getCol(words.get(i)))) return false;
        }
        cutRows();
        tables.pop();
        tables.push(current);
        return true;
    }

    boolean checkCond(String word) {
        switch(word) {
            case "=" : 
            case "<" :
            case ">" : 
            case "!=" : 
            case "<=" : 
            case ">=" : 
                      return true;
            default : System.err.println("Expression invalid.");
                      return false;
        }
    }

    boolean condMatch(String cond, int comp) {
        switch(cond) {
            case "=" : if(comp == 0) return true;
                       else return false;
            case "<" : if(comp < 0) return true;
                       else return false;
            case ">" : if(comp > 0) return true;
                       else return false;
            case "!=": if(comp != 0) return true;
                        else return false;
            case "<=": if(comp <= 0) return true;
                        else return false;
            case ">=": if(comp >= 0) return true;
                        else return false;
            default: return false;
        }
    }

    // Take only rows where the current condition is satisfied.
    boolean cmdRows(ArrayList<String> words) {
        if(words.size() < 4) {
            System.err.println("rows COLUMN <CONDITION> VALUE");
        }
        if(!checkCond(words.get(2))) return false;
        Table newcurrent = new Table();
        newcurrent.create(current.name);
        newcurrent.columns = current.columns;
        int index = current.columns.indexOf(current.getCol(words.get(1)));
        if(index == -1) {
            System.err.println("Column does not exist.");
            return false;
        }
        for(Record record : current.records) {
            int comp = record.get(index).compareTo(words.get(3));
            if(condMatch(words.get(2), comp)) newcurrent.add(record);
        }
        current = newcurrent;
        return true;
    }

    // Add a column to the current table.
    boolean cmdAppend(ArrayList<String> words) {
        if(words.size() < 3) {
            System.err.println("append COLUMN_NAME TYPE");
            return false;
        }
        if(current.length() != 0) {
            System.err.println("Can only append columns if table is empty.");
            return false;
        }
        return current.add(words.get(1), words.get(2), "na");
    }

    // Rename a column in the current table.
    boolean cmdRename(ArrayList<String> words) {
        if(words.size() != 3) {
            System.err.println("RENAME COLUMN_NAME NEW_COLUM_NAME");
            return false;
        }
        return current.rename(words.get(1), words.get(2));
    }

    // Add a row to the current table.
    boolean cmdInsert(ArrayList<String> words) {
        if(words.size() < current.size()+1) {
            System.err.println("insert VALUE_A VALUE_B ...");
            return false;
        }
        Record record = new Record();
        if(!record.create()) return false;
        for(int i=1; i<words.size(); i++) {
            if(!record.add(words.get(i))) return false;
        }
        return current.add(record);
    }

    // List the tables in the database, or the columns in the given table.
    boolean cmdList(ArrayList<String> words) {
        if(words.size() > 2) {
            System.err.println("list | list TABLE");
            return false;
        }
        if(words.size() == 1) display.list(database.list());
        else {
            Table table = database.get(words.get(1));
            if(table != null) display.list(table.list());
            else { 
                System.err.println("Table does not exist.");
                return false;
            }
        }
        return true;
    }

    // Add a key to the given column.
    boolean cmdKey(ArrayList<String> words) {
        if(words.size() < 3) {
            System.err.println("key COLUMN KEY_TYPE <foreign_reference>");
            return false;
        }
        if(words.get(2).equals("primary")) {
            current.addKey(words.get(1), "primary", "");
        }
        if(words.get(2).equals("foreign")) {
            if(words.size() < 4) {
                System.err.println("database-column must be referred to.");
                return false;
            }
            current.addKey(words.get(1), "foreign", 
                database.name+"-"+words.get(3));
        }
        if(words.get(2).equals("remove")) {
            current.addKey(words.get(1), "remove", "");
        }
        return true;
    }

    boolean columnCompare(Table t1, Table t2, int i, int j) {
        if(!t1.columns.get(i).name.equals(t2.columns.get(j).name)
            || !t1.columns.get(i).type.equals(t2.columns.get(j).type)
              || !t1.columns.get(i).reference.equals(
                   t2.columns.get(j).reference)) return false;
        else return true;
    }

    boolean tableCompare(Table t1, Table t2) throws Exception {
        for(int i=0; i<t1.size(); i++) {
            if(!columnCompare(t1, t2, i, i)) {
                revert(t1, t2, "Table columns do not match.");
                return false;
            }
        }
        return true;
    }

    // Reverts back to the state before the join/union started.
    void revert(Table t1, Table t2, String Reason) {
        if(t2!=null) tables.push(t2);
        if(t1!=null) tables.push(t1);
        System.err.println(Reason);
    }

    // Combine rows from two tables with matching columns.
    boolean cmdUnion(ArrayList<String> words) {
        if(words.size() > 1) {
            System.err.println("Union takes no arguments.");
            return false;
        }
        Table table1 = null;
        Table table2 = null;
        try {
            table1 = tables.pop();
            table2 = tables.pop();
        } catch(Exception e) { 
            revert(table1, table2, "Please select at least two tables first.");
            return false;
        }
        try {
            if(!tableCompare(table1, table2)) return false;
        } catch(Exception e) { 
            revert(table1, table2, "Table columns do not match.");
            return false;
        }
        for(Record record : table2.records) {
           table1.add(record);
        }
        tables.push(table1);
        return true;
    }

    private boolean joinRecords(Table newtable, Table table1, Table table2, 
      ArrayList<String> words) {
        for(Record rec1 : table1.records) {
            for(Record rec2 : table2.records) {
                Record newRec = new Record();
                newRec.create();
                for(int i=1; i<words.size(); i++) {
                   int i1 = table1.columns.indexOf(table1.getCol(words.get(i)));
                   int i2 = table2.columns.indexOf(table2.getCol(words.get(i)));
                    if(i1!=-1 && i2!=-1) {
                        if(rec1.get(i1).equals(rec2.get(i2))) {
                            newRec.add(rec1.get(i1));
                        }
                    }
                    else if((i1!=-1 && !newRec.add(rec1.get(i1)))
                        || (i2!=-1 && !newRec.add(rec2.get(i2)))) {
                        revert(table1, table2, "Internal error.");
                        return false;
                    }
                }
                if(newRec.size() == newtable.size()) newtable.add(newRec);
            }
        }
        return true;
    }

    private boolean assessColumns(Table newtable, Table table1, Table table2, 
      ArrayList<String> words) {
        ArrayList<String> matches = new ArrayList<String>();
        for(int i=1; i<words.size(); i++) {
            try {
                if(table1.getCol(words.get(i)).name.equals(
                       table2.getCol(words.get(i)).name)) {
                    matches.add(words.get(i));
                }
            } catch(Exception e) { } // Null columns get caught here.
            if(table1.getCol(words.get(i))!=null) {
                newtable.add(table1.getCol(words.get(i)));
            }
            else if(table2.getCol(words.get(i))!=null) {
                newtable.add(table2.getCol(words.get(i)));
            }
        }
        if(matches.isEmpty()) {
            revert(table1,table2,"Column names must match in both tables.");
            return false;
        }
        return true;
    }

    // Makes a new table from the join and pushes it.
    boolean constructJoin(Table table1, Table table2, ArrayList<String> words) {
        Table newtable = new Table();
        newtable.create(table1.name + "+" + table2.name);
        if(!assessColumns(newtable, table1, table2, words)) return false;
        if(!joinRecords(newtable, table1, table2, words)) return false;
        tables.push(newtable);
        current = newtable;
        return true;
    }

    // Joins the top two tables on matching column names.
    boolean cmdJoin(ArrayList<String> words) {
        if(words.size() < 2) {
            System.err.println("JOIN <OUTPUT COLUMNS> [joins on name matches]");
            return false;
        }
        Table table1 = null;
        Table table2 = null;
        try {
            table1 = tables.pop();
            table2 = tables.pop();
        } catch(Exception e) { 
            revert(table1, table2, "Please select at least two tables first.");
            return false;
        }
        if(!constructJoin(table1, table2, words)) return false;
        return true;
    }

    // List the column details of the given table.
    boolean cmdDetail(ArrayList<String> words) {
        Table table = database.get(words.get(1));
        if(table != null) display.list(table.detail());
        else System.err.println("Table does not exist.");
        return false; // So the table does not display again.
    }

    boolean readInput(ArrayList<String> words) {
        if(words.isEmpty()) return false;
        switch(words.get(0).toLowerCase()) {
            case "list" : return cmdList(words);
            case "detail" : return cmdDetail(words);
            case "table" : return cmdTable(words);
            case "new" : return cmdNew(words); 
        }
        if(current == null) {
            System.err.println("Please select a table first.");
            return false;
        }
        switch(words.get(0).toLowerCase()) {
            case "union" : return cmdUnion(words); 
            case "join" : return cmdJoin(words); 
            case "save" : return cmdSave(words);
            case "columns" : return cmdColumns(words); 
            case "rows" : return cmdRows(words); 
            case "rename" : return cmdRename(words); 
            case "append" : return cmdAppend(words); 
            case "insert" : return cmdInsert(words); 
            case "key" : return cmdKey(words); 
            default: System.err.print("Command '" + words.get(0));
                     System.err.println("' not recognised.");
                     return false;
        }
    }

    boolean acceptYN() {
        while(true) {
            String string = acceptLine().toLowerCase();
            if(string.equals("y") || string.equals("yes")) return true;
            if(string.equals("n") || string.equals("no")) return false;
            System.err.println("Yes or no?");
        }        
    }

    boolean userQuit(String word) { // A nicer way to terminate than CTRL+C
        if(word.equals("stop") || word.equals("quit")) return true;
        if(word.equals("end") || word.equals("close")) return true;
        if(word.equals("exit") || word.equals("kill")) return true;
        return false;
    }

    boolean confirmCreation(String name) {
        System.err.println("'" + name + "' does not yet exist.");
        System.err.println("Create it? y/n");
        return acceptYN();
    }

    boolean loadDatabase(String name) {
        File file = new File("./" + name);
        if(!file.isDirectory() && !confirmCreation(name)) return false;
        database.create(name);
        return database.load();
    }

    boolean readArgs(String[] args) {
        if(args.length < 1) {
            System.err.println("Please give the name of a database to manage.");
            return false;
        }
        if(args.length > 1) {
            System.err.println("Database names cannot be more than one word.");
            return false;
        }
        return loadDatabase(args[0]);
    }

    String acceptLine() {
        Scanner scan = new Scanner(System.in);
        return scan.nextLine();
    }

    void run(String[] args) {
        construct();
        if(!readArgs(args)) return;
        while(true) {
            String[] strings = acceptLine().split(" ");
            ArrayList<String> words = 
                new ArrayList<String>(Arrays.asList(strings));   
            if(userQuit(words.get(0))) return;
            if(readInput(words) && current != null) {
                current.order();
                display.printCurrent();
            }
        }
    }

    public static void main(String[] args) {
        DB db = new DB();
        db.run(args); // Let us leave this static realm.
    }
}
