## **HOW TO RUN THE CODE:**



1. Open the compiler on your system



2\. Change the current directory to the folder in which the source code file is kept.

 	(This can be done by pasting the following command in the MINGW64 terminal:

 		cd "PATH COPIED OF THE DIRECTORY")



3\. Enter the following command:

 	chmod +x run.sh (only once, to make the script executable)

 	./run.sh



4\. Now just give the inputs(in the format declared later). The runtime has started.



5\. Now we can see the output on the terminal. The above commands instantiate the run.sh file, which is also given in the folder, and it compiles the C++ code in the backend so that the user doesn't have to type all the commands to run the C++ file every time.



6\. **NOTE: ALTERNATIVE WAY TO RUN THE CODE:**
	AN APPLICATION FILE IS PROVIDED IN THE ZIPPED FOLDER WHICH DIRECTLY COMPILES THE CODE. THE USER CAN JUST OPEN IT AND BEGIN ENTERING THE COMMANDS TO TEST THE PROGRAM.



#### Commands (format) that the user can give:



The following are the commands that the program can execute:



##### a) Core File Operations



 	1)CREATE <filename> Creates a file with a root version (ID 0), empty content, and an initial snapshot

 	message. If the file with that name already exists in the system, the system outputs "FILE ALREADY EXISTS".



 	2)READ <filename> Displays the content of the file’s currently active version. If the file with that name doesn't exist in the system, the system outputs "FILE DOESN'T EXIST"



 	3)INSERT <filename> <content> Appends content to the file. This creates a new version if the active

 	version is already a snapshot; otherwise, it modifies the active version in place.

 	If the file with that name doesn't exist in the system, the system outputs "FILE DOESN'T EXIST"



 	4)UPDATE <filename> <content> Replaces the file’s content. Follows the same versioning logic as

 	INSERT



 	5)SNAPSHOT <filename> <message> Marks the active version as a snapshot, making its content im-

 	mutable. It stores the provided message and the current time.  If the file with that name doesn't exist in the system, the system outputs "FILE DOESN'T EXIST".

 	If the existing version is already a snapshot, it prints "ALREADY A SNAPSHOT".



 	6)ROLLBACK <filename> \[versionID] Sets the active version pointer to the specified versionID. If

 	no ID is provided, it rolls back to the parent of the current active version.  If the file with that name doesn't exist in the system, the system outputs "FILE DOESN'T EXIST"



 	7)HISTORY <filename> Lists all snapshotted versions of the file chronologically, which lie on the path from the active node to the root, showing their ID,

 	timestamp, and message.  If the file with that name doesn't exist in the system, the system outputs "FILE DOESN'T EXIST"



##### b) System-wide operations



 	1) RECENT FILES \[num] Lists files in descending order of their last modification time restricted to the

 	first num entries. If num is greater than the number of recent files, it just outputs all the recent files.



 	2) BIGGEST TREES \[num] Lists files in descending order of their total version count restricted to the first

 	num entries.  If num is greater than the number of files, it just outputs all the files.



\*\*If the user gives a command that is not in the above list of commands, the program outputs "UNKNOWN COMMAND:"<COMMAND> discards the rest of the line and continues with the execution of the program.



##### c) Key Semantics



 	•Immutability: Only snapshotted versions are immutable. Non-snapshotted versions can be edited in

 	place.



 	•Versioning: Version IDs are unique per file and assigned sequentially, starting from 0.



##### d) Sample input and output



input format : <COMMAND> <FILENAME> <VERSIONID/MESSAGE/CONTENT>



input:

 	CREATE file1

 	INSERT file1 Hello

 	INSERT file1 World

 	SNAPSHOT file1 "First snapshot"

 	READ file1

 	HISTORY file1

 	CREATE file2

 	INSERT file2 Data

 	SNAPSHOT file2 "Second file snapshot"

 	READ file2

 	RECENT\_FILES 3

 	BIGGEST\_TREES 3



output:

 	HelloWorld

 	Version ID: 1, Message: "First snapshot", Snapshot taken at: Wed Sep 10 10:26:47 2025



 	Version ID: 0, Message: Initial content, Snapshot taken at: Wed Sep 10 10:26:47 2025



 	Data

 	THE FILES IN THE DESCENDING ORDER OF RECENT MODIFICATION ARE:

 	file2

 	file1

 	THE FILES IN THE DESCENDING ORDER OF NUMBER OF VERSIONS ARE:

 	file2

 	file1



NOTE: IN CASE THE "commands.txt" file is empty the user can also give input in the stdin terminal by typing the above commands. The main function handles this correctly.



#### Description of the code:



First of all, the code is segregated into 2 parts: the implementation of specific classes and the main function to run the code.



##### Architecture and Data Structures



\- Tree node (per version): class TreeNode

  - Fields: version\_id (int), content (string), message (string),

    created\_timestamp (time\_t), snapshot\_timestamp (time\_t),

    parent (TreeNode\*), children (vector<TreeNode\*>).

  - Purpose:

    - Represents a version in a file’s version tree.

    - children builds the version DAG (actually a tree here).

  - Used by: all per‑file operations; HISTORY walks parent pointers.



\- Version index (version\_id → node): class IntToNodeMap

  - Type: custom hash map with separate chaining.

  - Storage: vector<vector<pair<int, TreeNode\*>>> (buckets).

  - Hash: abs(key) % cap; rehash at ~0.75 load factor.

  - Used by: ROLLBACK <versionId> (O(1) avg); version creation inserts here.



\- File table (filename → VersionControlSystem\*): class StrToVcsMap

  - Type: custom hash map with separate chaining.

  - Storage: vector<vector<pair<string, VersionControlSystem\*>>>.

  - Hash: polynomial rolling (base 131), mod cap; rehash at ~0.75.

  - Used by: all file‑level commands to locate the file VCS (O(1) avg).

  - items(): flattens entries to a vector for analytics queries.



\- Max‑heaps for analytics:

  - priority\_queue\_custom1 (pair<int,string>) for BIGGEST\_TREES.

  - priority\_queue\_custom2 (pair<time\_t,string>) for RECENT\_FILES.

  - Storage: vector<Pair> as a binary max‑heap; sift‑up/down on push/pop.

  - Comparison: uses pair’s operator< (lexicographic: first, then second) via x\[a] < x\[b].



\- file controller: class VersionControlSystem(Controller of individual files)

  - Fields: root, active\_version (TreeNode\*), version\_map (IntToNodeMap),

    total\_versions (int), last\_modified\_time (time\_t).

  - Behavior:

    - INSERT:

      - If no snapshot yet on active: in‑place append to content.

      - Else: create new TreeNode child, link via addChild, update active\_version,

        insert into version\_map, increment total\_versions.

      - Updates last\_modified\_time.

    - UPDATE: same split behavior as INSERT; replaces content.

    - SNAPSHOT: stamps active version with message + snapshot\_timestamp now.

    - ROLLBACK <id>: lookup in version\_map; switch active\_version on hit or print “INVALID VERSION ID”.

    - ROLLBACK: move to parent; if none, print “NO PARENT VERSION EXISTS” and reset to version 0; updates last\_modified\_time.

    - HISTORY:

      - Collects snapshot nodes along ancestor chain to root.

      - Sorts by snapshot\_timestamp and prints with ctime.



\- Top‑level manager: class FileSystem

  - Holds files (StrToVcsMap).

  - Dispatches commands to each file’s VersionControlSystem.

  - RECENT\_FILES:

    - Builds a heap of {last\_modified\_time, filename} from files.items().

    - Pops N times; prints filenames (most recent first).

  - BIGGEST\_TREES:

    - Builds a heap of {version\_count, filename}; pops N.



##### DATA STRUCTURES IMPLEMENTED FROM SCRATCH



* ###### class priority\_queue\_custom1{}-



This data structure is used in the BIGGEST\_TREES class. This class essentially follows the implementation of the priority queues in  C++ using the max-heap. The root of the heap stores the filename with the maximum version count. The heap stores a pair of the version number and the filename.



* ###### class priority\_queue\_custom2{}-



This data structure is used in the RECENT\_FILES class. This class essentially follows the implementation of the priority queues in  C++ using the max-heap. The root of the heap stores the filename with the maximum timestamp count. The heap stores a pair of the timestamp and the filename.

###### 

* ###### class  IntToNodeMap{}-



This is basically the implementation of the hashmap that maps the version count to the file node pointer.

Features:

 	Separate chaining for collisions

 	Dynamic resizing at load factor 0.75

 	Integer keys, TreeNode\* values



* ###### class StrToVcsMap{}-



This StrToVcsMap is a custom hash map that maps a string key to a VersionControlSystem\* value.



Features:

 	Hash function (hs) - uses a polynomial rolling hash with base 131, then reduces modulo cap.



 	Dynamic resizing → when load factor ≥ 0.75, it doubles capacity (rehash).



 	has(key) - check if a key exists.



 	get(key) - retrieve the mapped VersionControlSystem\* (or nullptr if missing).



 	put(key, val) - insert or update mapping.



 	items() - flatten the whole map into a single vector of (key, value) pairs (useful for iteration).



#### Complexity



\- Maps (custom hash maps): average O(1) get/put (worst case O(n)).

\- Heaps in queries: O(log F) per push/pop, where F is the number of files.

\- Operations:

  - CREATE/READ/INSERT/UPDATE/SNAPSHOT/ROLLBACK: O(1) average.

  - HISTORY: O(H + S log S), H = path length to root, S = number of snapshots on that path.

