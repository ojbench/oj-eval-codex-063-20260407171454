#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>
#include <ios>
#include <string>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

// A simple file-based storage with free-list space reclamation.
// We reserve the first `info_len` ints for user info (1-based indexing in get_info/write_info).
// Internally, we add one extra header int right after the user header to store the free-list head (byte offset of a free block, or 0 if none).
// Objects are stored starting at offset (info_len + 1) * sizeof(int).
// When a block is deleted, we store the previous free head (an int offset) into the first sizeof(int) bytes of that block,
// and set the free head to point to this block. On allocation, we pop from this free list; otherwise we append at EOF.
template<class T, int info_len = 2>
class MemoryRiver {
private:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);

    // Offset helpers
    static constexpr std::streamoff user_header_bytes() { return static_cast<std::streamoff>(info_len) * sizeof(int); }
    static constexpr std::streamoff internal_header_bytes() { return static_cast<std::streamoff>(info_len + 1) * sizeof(int); }
    static constexpr std::streamoff free_head_offset() { return static_cast<std::streamoff>(info_len) * sizeof(int); }
    static constexpr std::streamoff data_region_offset() { return static_cast<std::streamoff>(info_len + 1) * sizeof(int); }

    void open_rw() {
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            // Try to create the file if it doesn't exist
            ofstream create(file_name, std::ios::binary);
            create.close();
            file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        }
    }

    // Ensure the internal header (extra int for free list head) exists
    void ensure_internal_header() {
        file.seekg(0, std::ios::end);
        std::streamoff size = file.tellg();
        if (size < internal_header_bytes()) {
            // Extend file to internal_header_bytes, writing zeros for missing ints
            file.clear();
            file.seekp(0, std::ios::end);
            int zero = 0;
            // Write missing user header ints if needed (initialise() should usually do this)
            while (size < user_header_bytes()) {
                file.write(reinterpret_cast<char*>(&zero), sizeof(int));
                size += sizeof(int);
            }
            // Write the extra internal int (free list head)
            while (size < internal_header_bytes()) {
                file.write(reinterpret_cast<char*>(&zero), sizeof(int));
                size += sizeof(int);
            }
            file.flush();
        }
    }

    int read_free_head() {
        open_rw();
        ensure_internal_header();
        file.seekg(free_head_offset(), std::ios::beg);
        int head = 0;
        file.read(reinterpret_cast<char*>(&head), sizeof(int));
        file.close();
        return head;
    }

    void write_free_head(int head) {
        open_rw();
        ensure_internal_header();
        file.seekp(free_head_offset(), std::ios::beg);
        file.write(reinterpret_cast<char*>(&head), sizeof(int));
        file.close();
    }

public:
    MemoryRiver() = default;

    MemoryRiver(const string& file_name) : file_name(file_name) {}

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out | std::ios::binary);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        // Also write our internal extra header int (free list head)
        file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    // 1-based: read the nth int header value into tmp
    void get_info(int &tmp, int n) {
        if (n > info_len || n <= 0) return;
        open_rw();
        file.seekg(static_cast<std::streamoff>(n - 1) * sizeof(int), std::ios::beg);
        file.read(reinterpret_cast<char*>(&tmp), sizeof(int));
        file.close();
    }

    // 1-based: write tmp into the nth int header slot
    void write_info(int tmp, int n) {
        if (n > info_len || n <= 0) return;
        open_rw();
        file.seekp(static_cast<std::streamoff>(n - 1) * sizeof(int), std::ios::beg);
        file.write(reinterpret_cast<char*>(&tmp), sizeof(int));
        file.close();
    }
    
    // Write object t into file; return the byte offset index where it was stored
    int write(T &t) {
        int head = read_free_head();
        if (head != 0) {
            // Reuse a freed slot; pop from free list
            open_rw();
            ensure_internal_header();
            // Read next from the freed block's first sizeof(int)
            int next = 0;
            file.seekg(static_cast<std::streamoff>(head), std::ios::beg);
            file.read(reinterpret_cast<char*>(&next), sizeof(int));

            // Update free list head
            file.seekp(free_head_offset(), std::ios::beg);
            file.write(reinterpret_cast<char*>(&next), sizeof(int));

            // Write the object into this slot
            file.seekp(static_cast<std::streamoff>(head), std::ios::beg);
            file.write(reinterpret_cast<char*>(&t), sizeof(T));
            file.close();
            return head;
        } else {
            // Append at EOF; ensure header exists
            open_rw();
            ensure_internal_header();
            file.seekp(0, std::ios::end);
            std::streamoff pos = file.tellp();
            if (pos < data_region_offset()) {
                // If the file was just header, start from data region
                file.seekp(data_region_offset(), std::ios::beg);
                pos = data_region_offset();
            }
            file.write(reinterpret_cast<char*>(&t), sizeof(T));
            file.close();
            return static_cast<int>(pos);
        }
    }
    
    // Overwrite the object at the given index
    void update(T &t, const int index) {
        open_rw();
        ensure_internal_header();
        file.seekp(static_cast<std::streamoff>(index), std::ios::beg);
        file.write(reinterpret_cast<char*>(&t), sizeof(T));
        file.close();
    }
    
    // Read the object at the given index
    void read(T &t, const int index) {
        open_rw();
        ensure_internal_header();
        file.seekg(static_cast<std::streamoff>(index), std::ios::beg);
        file.read(reinterpret_cast<char*>(&t), sizeof(T));
        file.close();
    }
    
    // Delete the object at index and add its space to the free list
    void Delete(int index) {
        // Read current head
        int head = read_free_head();
        open_rw();
        ensure_internal_header();
        // Write current head into the freed block's first bytes
        file.seekp(static_cast<std::streamoff>(index), std::ios::beg);
        file.write(reinterpret_cast<char*>(&head), sizeof(int));
        // Update free head to this index
        file.seekp(free_head_offset(), std::ios::beg);
        int new_head = index;
        file.write(reinterpret_cast<char*>(&new_head), sizeof(int));
        file.close();
    }
};


#endif //BPT_MEMORYRIVER_HPP
