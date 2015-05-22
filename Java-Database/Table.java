import java.util.HashSet;
import java.util.ArrayList;
import java.util.Iterator;
import java.io.File;
import java.util.Scanner;
import java.io.PrintWriter;
import java.util.regex.PatternSyntaxException;

class Table {
    String name;
    ArrayList<Column> columns;
    ArrayList<Record> records;
    ArrayList<Integer> pkColumn;

    class Column {
        String name;
        String type;
        String reference; // 'pk' or 'database-table-column' for foreign key
        int width;

        void construct(String name, String type, String reference) {
            this.name = name;
            this.type = type;
            this.reference = reference;
            this.width = name.length();
        }
    }

    private void construct(String title) {
        columns = new ArrayList<Column>();
        records = new ArrayList<Record>();
        pkColumn = new ArrayList<Integer>();
        name = title;
    }

    private boolean columnExists(String name) { // If it does it is an error
        if(columns.isEmpty()) return false;
        for(Column existing : columns) {
            if(existing.name.equals(name)) return true;
        }
        return false;
    }

    // Finds the maximum size of entry in the given column.
    private int findWidth(Column col) {
        int max = col.name.length();
        int index = columns.indexOf(col);
        for(Record rec : records) {
            int len = rec.get(index).length();
            if(len > max) max = len;
        }
        return max;
    }

    private void setWidths() {
        for(Column col : columns) {
            col.width = findWidth(col);
        }
    }

    // Returns the column with the given name.
    Column getCol(String name) {
        if(columns.isEmpty()) return null;
        for(Column column : columns) {
            if(column.name.equals(name)) return column;
        }
        return null;
    }

    // Returns the column with the given reference (possibly from another table)
    private Column getColByRef(String reference) {
        String[] dirs = reference.split("-");
        Table table = new Table();
        if(dirs.length < 3) return null;
        Column col = null;
        if(dirs[1].equals(this.name)) col = getCol(dirs[2]);
        else {
            if(!table.load(dirs[0], dirs[1])) return null;
            col = table.getCol(dirs[2]);
        }
        return col;
    }

    // Prints the uniquely-defining keys of the given record.
    private void printKeys(Record record) {
        for(int i : pkColumn) {
            System.err.println(record.get(i) + "; ");
        }
    }

    // Returns an ArrayList of uniquely-defining keys of the given record.
    private ArrayList<String> getKey(Record record) {
        ArrayList<String> keys = new ArrayList<String>();
        for(int i : pkColumn) {
            keys.add(record.get(i));
        }
        return keys;
    }

    // Finds whether the record matches the key.
    private boolean keyMatch(ArrayList<String> key, Record record) {
        int match = 1, j=0; // j iterates through key
        for(int i : pkColumn) {
            if(record.get(i).equals(key.get(j++))) match = match*1;
            else match = match*0;
        }
        if(match == 1) return true;
        else return false;
    }

    private boolean checkFloat(String string) {
        try { 
            Float.parseFloat(string);
        } catch(Exception e) { return false; }
        return true;
    }

    private boolean checkInteger(String string) {
        try { 
            Integer.parseInt(string);
        } catch(Exception e) { return false; }
        return true;
    }

    private boolean dateError(String error) {
        System.err.println("Date error: " + error);
        return false;
    }

    private boolean checkValidDate(int[] YMD) {
        if(YMD[0] < 0) return dateError("Year is invalid.");
        if(YMD[1] < 0 || YMD[1] > 12) return dateError("Month is invalid.");
        if(YMD[2] < 0) return dateError("Day is invalid.");
        int[] shorts = {4, 6, 9, 11}; // Months with 30 days.
        for(int m : shorts) {
            if(YMD[1] == m && YMD[2] > 30) {
                return dateError("Day is invalid with month.");
            }
        }
        if(YMD[1] == 2) { // Everyone hates February
            if(YMD[2] > 29) return dateError("Day is invalid with month.");
            if(YMD[0]%4 != 0 && YMD[2] > 28) { 
                return dateError("Day is invalid with year (no leap year).");
            }
        }
        return true;
    }

    private boolean checkDate(String string) {
        if(!checkIntSplit(string, '-', 4, 2, 2)) {
            System.err.println("Date takes YYYY-MM-DD.");
            return false;
        }
        return true;
    }

    private boolean timeError(String error) {
        System.err.println("Time error: " + error);
        return false;
    }

    private boolean checkValidTime(int[] HMS) {
        if(HMS[0] < 0 || HMS[0] >= 24) return timeError("Hours is invalid.");
        if(HMS[1] < 0 || HMS[1] >= 60) return timeError("Minutes is invalid.");
        if(HMS[2] < 0 || HMS[2] >= 60) return timeError("Seconds is invalid.");
        return true;
    }

    private boolean checkTime(String string) {
        if(!checkIntSplit(string, ':', 2, 2, 2)) {
            System.err.println("Time takes HH:MM:SS");
            return false;
        }
        return true;
    }

    // Checks whether integers have been given split by c with lengths x y z.
    private boolean checkIntSplit(String string, char c, int x, int y, int z) {
        String[] XYZ = string.split("" + c);
        if(XYZ.length != 3) return false;
        if(XYZ[0].length() != x || XYZ[1].length() != y 
             || XYZ[2].length() != z) return false;
        for(String num : XYZ) {
            if(!checkInteger(num)) return false;
        }
        int[] ints = new int[XYZ.length];
        for(int i=0; i<XYZ.length; i++) ints[i] = Integer.parseInt(XYZ[i]);
        if(c=='-') return checkValidDate(ints);
        if(c==':') return checkValidTime(ints);
        return true;
    }

    // Find the column referred to by the given column's references.
    private boolean checkForeignType(String entry, Column column) {
        Column ref = getColByRef(column.reference);
        if(ref == null) return false;
        else return typeMatch(entry, ref);
    }

    boolean RType(int i) { // Finds if the ith column should be right-aligned
        String type = columns.get(i).type;
        if(type.equals("integer") || type.equals("float")) return true;
        return false;
    }

    private boolean validType(String type) {
        if(type.equals("string") || type.equals("integer")) return true;
        if(type.equals("float")) return true;
        if(type.equals("foreign")) return true;
        if(type.equals("date")) return true;
        if(type.equals("time")) return true;
        System.err.println("Type " + type + " is not valid.");
        return false;
    }

    // Finds whether the type of the given column matches the given entry.
    private boolean typeMatch(String entry, Column column) {
        switch(column.type) {
            case "string" : return true;
            case "integer" : return checkInteger(entry);
            case "float" : return checkFloat(entry);
            case "foreign" : return checkForeignType(entry, column);
            case "date" : return checkDate(entry);
            case "time" : return checkTime(entry);
            default : return true;
        }
    }

    // Invalid entries don't match types, or already exist.
    private boolean validEntry(String entry, Column column) {
        int slash = entry.indexOf('/'); // first occurance of slash
        int newline = entry.indexOf('\n'); // first occurance of newline
        if(slash != -1 || newline != -1) {
            System.err.print("Invalid entry containing ");
            System.err.println("forbidden characters: '" + entry + "'");
            return false;
        }
        if(!typeMatch(entry, column)) {
            System.err.print("Invalid entry '" + entry);
            System.err.println("' for column of type " + column.type);
            return false;
        }
        return true;
    }

    // Finds whether the record under the given key already exists.
    private boolean keyExists(ArrayList<String> key) {
        Record record = get(key);
        if(record == null) return false;
        else return true;
    }

    // Finds whether the reference refers to a column entry that actually exists
    private boolean validForeign(Record record) {
        for(Column column : columns) {
            if(column.type.equals("foreign")) {
                String[] ref = column.reference.split("-");
                Table table = new Table();
                if(!table.load(ref[0], ref[1])) return false;
                ArrayList<String> fk = new ArrayList<String>();
                fk.add(record.get(columns.indexOf(column)));
                if(table.get(fk) == null) {
                    System.err.print("Foreign key does not exist: ");
                    System.err.print(fk.get(0));
                    return false;
                }
            }
        }
        return true;
    }

    // Checks size, primary key or foreign references, and entry itself (above).
    private boolean validRecord(Record record) {
        if(record.size() > size()) {
            System.err.println("Record incompatible with table.");
            record.print();
            return false;
        }
        if(!pkColumn.isEmpty() && keyExists(getKey(record))) {
            System.err.print("Record already exists: ");
            printKeys(record);
            return false;
        }
        if(!validForeign(record)) return false;
        for(int i=0; i<record.size(); i++) {
            if(!validEntry(record.get(i), columns.get(i))) return false;
        }
        return true;
    }

    private boolean checkForeign(Column column) {
        Column ref = getColByRef(column.reference);
        if(ref == null) return false;
        else return true;
    }

    // Checks whether any value in the given column is repeated (for new keys)
    private boolean hasDuplicate(Column column) {
        HashSet<String> set = new HashSet<String>();
        for(Record record : records) {
            if(!set.add(record.get(columns.indexOf(column)))) return true;
        }
        return false;
    }

    private String newType() {
        Scanner scan = new Scanner(System.in);
        String string = scan.nextLine();
        if(!validType(string)) {
            System.err.println("Invalid type.");
            return null;
        }
        return string;
    }

    private void addPrimary(Column col) {
        col.reference = "pk";
        if(hasDuplicate(col)) {
            System.err.println("Column has duplicates; cannot be primary.");
            return;
        }
        pkColumn.add(columns.indexOf(col));
    }

    private void addForeign(Column col, String key, String reference) {
        String prevRef = col.reference;
        col.reference = reference;
        String prevType = col.type;
        col.type = key;
        for(Record record : records) {
            if(!validForeign(record)) {
                col.reference = prevRef;
                col.type = prevType;
                System.err.println("Some record references do not exist.");
                return;
            }
        }
    }

    private void removeKey(Column col, String reference) {
        if(col.type.equals("foreign")) {
            col.type = getColByRef(col.reference).type;
        }
        col.reference = "na";
        int index = pkColumn.indexOf(columns.indexOf(col));
        if(index != -1) pkColumn.remove(index);
    }

    void addKey(String name, String key, String reference) {
        Column col = getCol(name);
        if(col==null) {
            System.err.println("Column does not exist: " + name);
        }
        if(key.equals("primary")) addPrimary(col);
        if(key.equals("foreign")) addForeign(col, key, reference);
        if(key.equals("remove")) removeKey(col, reference);
    }

    boolean add(Column column) {
        return add(column.name, column.type, column.reference);
    }

    boolean add(String name, String type, String reference) {
        if(columnExists(name)) {
            System.err.println("Column already exists: " + name);
            return false;
        }
        if(!validType(type)) return false;
        Column column = new Column();
        column.construct(name, type, reference);
        if(type.equals("foreign") && !checkForeign(column)) return false;
        columns.add(column);
        if(reference.equals("pk")) {
            pkColumn.add(columns.indexOf(column));
        }
        return true;
    }

    boolean add(Record record) {
        if(!validRecord(record)) return false;
        records.add(record);
        setWidths();
        return true;
    }

    boolean add(int i, Record record) {
        if(!validRecord(record)) return false;
        records.add(i, record);
        setWidths();
        return true;
    }

    void remove(int i) {
        records.remove(i);
        setWidths();
    }

    void remove(Record record) {
        records.remove(record);
        setWidths();
    }

    void removeCol(int i) {
        columns.remove(i);
        for(Record record : records) {
            record.remove(i);
        }
    }

    void remove(ArrayList<String> key) {
        Record record = get(key);
        if(record != null) remove(record);
    }

    boolean validRename(String oldName, String newName) {
        if(columnExists(newName) || (oldName!=null && !columnExists(oldName))) {
            System.err.println("Cannot rename; check existing columns.");
            return false;
        }
        if(newName.indexOf("/") != -1 || newName.indexOf("\n") != -1 || newName.indexOf("-") != -1) {
            System.err.println("Invalid entry: " + newName);
            return false;
        }
        return true;
    }

    boolean rename(String oldName, String newName) {
        if(validRename(oldName, newName)) {
            getCol(oldName).name = newName;
            return true;
        }
        else return false;
    }

    boolean rename(int i, String newName) {
        if(validRename(null, newName)) {
            columns.get(i).name = newName;
            return true;
        }
        else return false;
    }

    private Record get(ArrayList<String> key) {
        for(Record record : records) {
            if(keyMatch(key, record)) {
            return record;
            }
        }
        return null;
    }

    Record get(int i) {
        return records.get(i);
    }

    int size() { // Returns the number of columns in the table.
        return columns.size();
    }

    int length() { // Returns the number of records in the table.
        return records.size();
    }

    String name(int i) {
        return columns.get(i).name;
    }

    // Returns an ArrayList of column names of this table.
    ArrayList<String> list() {
        ArrayList<String> names = new ArrayList<String>();
        for(Column column : columns) {
            names.add(column.name);
        }
        return names;
    }

    // Returns an ArrayList of names+types, etc for this table's columns.
    ArrayList<String> detail() {
        ArrayList<String> details = new ArrayList<String>();
        for(Column column : columns) {
            String str = column.name + ": TYPE " + column.type;
            if(column.type.equals("foreign")) {
                str = str.concat(", REF " + column.reference);
            }
            if(column.reference.equals("pk")) {
                str = str.concat(", PRIMARY KEY");
            }
            details.add(str);
        }
        return details;
    }

    int colComp(Record a, Record b, int limit) {
        for(int i=0; i<=limit; i++) {
            int c = a.get(i).compareTo(b.get(i));
            if(c!=0) return c;
        }
        return 0;
    }

    boolean colMatch(Record a, Record b, int limit) {
        for(int i=0; i<=limit; i++) {
            if(a.get(i).compareTo(b.get(i))!=0) return false;
        }
        return true;
    }

    // Finds the given record's place in order.
    private void insertNext(int colIndex, Record rec, int index, int limit,
                              ArrayList<Record> ordered) {
        while(index != limit 
                && colComp(rec, ordered.get(index), colIndex)>0) index++;
        if(index == limit) {
            ordered.add(rec);
            return;
        }
        int i=0; // i will be the number of matches we get
        while(index+i != limit
                && colMatch(rec, ordered.get(index+i), colIndex)) i++;

        if(i!=0 && colIndex<this.size()-1) {
            ArrayList<Record> subCollection = new ArrayList<Record>();
            for(int j=0; j<i; j++) {
                subCollection.add(ordered.get(index+j));
            }
            for(int j=i-1; j>=0; j--) {
                ordered.remove(ordered.get(index+j));
            }
            subCollection.add(rec);
            subCollection = orderCol(colIndex+1, 0, index+i-1, subCollection);
            i=0;
            for(Record r : subCollection) {
                ordered.add(index + i++, r);
            }
            
            return;
        }
        ordered.add(index, rec);
    }

    private ArrayList<Record> orderCol(int colIndex, int top, int bottom,
                                         ArrayList<Record> collection) {
        ArrayList<Record> ordered = new ArrayList<Record>();
        Iterator<Record> it = collection.iterator();
        if(it.hasNext()) ordered.add(0, it.next());
        while(it.hasNext()) {
            insertNext(colIndex, it.next(), 0, ordered.size(), ordered);
        }
        return ordered;
    }

    void order() {
        records = orderCol(0, 0, records.size(), records);
    }

    // Scans the rest of the file for records.
    private boolean scanRecord(File file) throws Exception {
        Scanner scan = new Scanner(file);
        scan.nextLine();
        while(scan.hasNextLine()) {
            Record record = new Record();
            if(!record.load(scan.nextLine()) || !add(record)) return false;
        }
        scan.close();
        return true;
    }

    // Interprets the file's top line into names, types and references.
    private boolean readHeader(String line) {
        String[] Header = line.split("/");
        for(int i=0; i<Header.length; i+=3) {
            if(!add(Header[i], Header[i+1], Header[i+2])) return false;
        }
        return true;
    }

    // Scans the column names, types and references from the file's top line.
    private boolean scanHeader(File file) throws Exception {
        String[] Header;
        Scanner scan = new Scanner(file);
        if(scan.hasNextLine()) {
            if(!readHeader(scan.nextLine())) return false;
        }
        return true;
    }

    // Loads this table from the given database(dir) and title(filename).
    boolean load(String dir, String title) {
        construct(title);      
        File file = new File("./" + dir + "/" + title + ".txt");
        if(!file.exists() || file.isDirectory()) {
            System.err.println("Error loading " + dir + "-" + title);
            return false;
        }
        try {
            if(!scanHeader(file) || !scanRecord(file)) return false;
        } catch(Exception e) { 
            System.err.println("This table is empty.");
        }
        return true;
    }

    // Saves this table in the given database(dir) and title(filename).
    void save(String dir, String title) {
        File file = new File("./" + dir + "/" + title + ".txt");
        PrintWriter writer = null;
        try {
            writer = new PrintWriter(file, "UTF-8");
        } catch(Exception e) { throw new Error(e); }

        for(Column column : columns) {
            writer.print(column.name + "/"
                           + column.type + "/" 
                             + column.reference + "/");
        }
        writer.println();
        for(Record record : records) {
            record.save(writer);
        }

        writer.close();
    }

    void create(String title) {
        construct(title);
    }
}

