import java.util.ArrayList;
import java.io.PrintWriter;

class Record {
    ArrayList<String> entries;

    private void construct() {
        entries = new ArrayList<String>();
    }

    private boolean validEntry(String entry) {
        if(entry.indexOf("/") != -1 || entry.indexOf("\n") != -1) {
            System.err.println("Entries cannot contain / or newlines.");
            return false;
        }
        return true;
    }

    boolean add(int i, String entry) {
        if(!validEntry(entry)) return false;
        entries.add(i, entry);
        return true;
    }

    boolean add(String entry) {
        if(!validEntry(entry)) return false;
        entries.add(entry);
        return true;
    }

    void remove(int i) {
        entries.remove(i);
    }

    boolean set(int i, String entry) {
        if(!validEntry(entry)) return false;
        entries.set(i, entry);
        return true;
    }

    String get(int i) {
        return entries.get(i);
    }

    int size() { // The number of columns this record fits.
        return entries.size();
    }

    void print() {
        for(String entry : entries) {
            System.err.print(entry + "|");
        }
        System.err.println();
    }

    // Loads this record from the given line.
    boolean load(String line) {
        construct();
        String[] strings = line.split("/");
        for(String entry : strings) {
            if(!add(entry)) return false;
        }
        return true;
    }

    // Writes this record as a single line into the given writer.
    void save(PrintWriter writer) {
        for(String entry : entries) {
            writer.print(entry + "/");
        }
        writer.println();
    }

    boolean create(String... strings) {
        construct();
        for(String string : strings) {
            if(!add(string)) return false;
        }
        return true;
    }
}

