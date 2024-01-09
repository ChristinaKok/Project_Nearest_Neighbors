# Ανάπτυξη Λογισμικού για Αλγοριθμικά Προβλήματα
## Εργασία 3, Χειμερινό Εξάμηνο 2023-24

Δρακοπούλου Ευγενία, 1115201900054  <br>
Κοκκινάκη Χριστίνα, 1115202000083   <br>

Link repository github: https://github.com/ChristinaKok/Project3

Τα αρχεία που περιλαμβάνονται στον φάκελο είναι:
    
    - LSH: main_lsh.cpp, lsh.cpp , lsh.h 
    - Hypercube: main_hypercube.cpp , hypercube.cpp , hypercube.h
    - Cluster: main_cluster.cpp , main_cluster_red.cpp , clusters.cpp , clusters.h , cluster.conf
    - GNNS/Search on graph (MRNG): main_graph.cpp , main_graph_red.cpp , graph.cpp , graph.h
    - Αρχεία με έτοιμους γράφους: graph_MRNG.txt , graph_gnns.txt , graph_MRNG_red.txt , graph_gnns_red.txt
    - Αρχεία εισόδου αρχικής διάστασης: train-images.idx3-ubyte , t10k-images.idx3-ubyte
    - Αρχεία εισόδου μειωμένης διάστασης: train_reduced.txt , query_reduced.txt 
    - Makefile
    - Αρχείο με νευρωνικό δίκτυο: reduce.py
    - Αρχείο με αποτελέσματα πειραμάτων: results.txt
    - Αρχεία με συναρτήσεις ανάγνωσης αρχείων εισόδου: functions.cpp , functions.h
    - Αρχείο αναφοράς: report.pdf

Ερώτημα Α

Περιγραφή: Υλοποίηση νευρωνικού δικτύου αυτοκωδικοποίησης (autoencoder) εικόνων με σκοπό την αναπαράσταση τους σε νέο διανυσματικό χώρο. 

Εκτέλεση: $python reduce.py –d (dataset) -q (queryset) -od (output_dataset_file) -oq (output_query_file)

Μετά από πειραματισμούς καταλήξαμε στην δομή νευρωνικού δικτύου που περιλαμβάνει το αρχείο reduce.py 

Επεξήγηση κώδικα: Αρχικά εκπαιδεύει το νευρωνικό δίκτυο με το dataset που δόθηκε από την γραμμή εντολών. Στην συνέχεια αποθηκεύει το μοντέλο autoencoder και κρατάει τις παραμέτρους που αφορούν τον encoder προκειμένου να συμπιέσει τις εικόνες του dataset και του queryset. 

Οι συντεταγμένες των διανυσμάτων είναι τύπου double ώστε να μην χαθεί πληροφορία.



Ερώτημα Β

Εκτος από τα παραδοτέα της 2ης εργασίας έχει προστεθεί και το αρχείο main_graph_red.cpp ώστε να υποστηρίζει τα αρχεία μειωμένης διάστασης.

Μεταγλώττιση: make graph_search2  (μεταγλώττιση αρχείων για είσοδο μειωμένης διάστασης)

Εκτέλεση: ./graph_search2 -d (dataset) -q (queryset) -o (output_file) -rd (reduced_dataset) -rq (reduced_queryset) -E (int) -R (int) -k (int) -N(int) -l (int) -m (int) -init (optional)

-init: Για δημιουργία και αρχικοποίηση νέων γράφων.

Νέα συνάρτηση:
    * convert_init: Αναγωγή σημείων στην αρχική διάσταση και κατάλληλη ενημέρωση των αποστάσεων των γειτόνων.



Ερώτημα Γ

Εκτος από τα παραδοτέα της 1ης εργασίας έχει προστεθεί και το αρχείο main_cluster_red.cpp ώστε να υποστηρίζει τα αρχεία μειωμένης διάστασης.

Μεταγλώττιση: make cluster2  (μεταγλώττιση αρχείων για είσοδο μειωμένης διάστασης)

Εκτέλεση: ./cluster -i (input_file) -ri (reduced_input_file) -c (configuration_file) -o (output_file) -m (Classic ή LSH ή Hypercube) -complete (optional)

Νέες συναρτήσεις:
    
    * silhouette_red: Υπολογισμός δείκτη silhouette έπειτα από αναγωγή των σημείων στον αρχικό χώρο. 
    
    * convert: Υπολογισμός νέου centroid κατά την αναγωγή στον αρχικό χώρο.
    
    * objective_function: Υπολογισμός συνάρτησης στόχου.



Αναφορά:
    Τα αποτελέσματα των πειραμάτων , ο σχολιασμός τους και η σύγκριση τους παρουασιάζονται στο αρχείο report.pdf και τα αποτελέσματα των εκτελέσεων φαίνονται αναλυτικά στο αρχείο results.txt . Όλα τα πειράματα έχουν γίνει στα linux της σχολής. 
