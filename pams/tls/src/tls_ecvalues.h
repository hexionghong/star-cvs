/*   tls_ecvalues.h    generated by ecvalsc.ace
 *  Condition value parameters for package tls,   21-Jun-1994     14:17:46
 *     Error codes for Sort/Search routines.
 *     
 */

/*** TLS_SORT_NORMAL_CV: Sort or Search routine successful   */
/*      */
#define TLS_SORT_NORMAL_CV  3585

/*** TLS_SORT_ID_SHORT_CV: Array passed to Id_Offsets for indices i   */
/*   s too small.        Some rows not included in index table.   */
#define TLS_SORT_ID_SHORT_CV  3844

/*** TLS_SORT_ID_MULTIPLE_CV: Table has two or more keys with same val   */
/*   ue. (Id_Offsets)   */
#define TLS_SORT_ID_MULTIPLE_CV  10498

/*** TLS_SORT_IS_SHORT_CV: Array for sorted indices too short (Inde   */
/*   x_Sort)   */
#define TLS_SORT_IS_SHORT_CV  10756

/*** TLS_SORT_QS_LONGROW_CV: Temporary storage space within quicksort   */
/*    too short.         (Quick_Sort)   */
#define TLS_SORT_QS_LONGROW_CV  11012

/*** TLS_SORT_QS_STACKSHORT_CV: Stack too short within quicksort (need r   */
/*   ecompile).          (Quick_Sort)   */
#define TLS_SORT_QS_STACKSHORT_CV  11268

/*** TLS_SORT_S_LISTSHORT_CV: Array to hold indices of matched keys to   */
/*   o short. (Search)   */
#define TLS_SORT_S_LISTSHORT_CV  11522

/*** TLS_SORT_S_NOMATCH_CV: Key not found in table. (Search)   */
/*      */
#define TLS_SORT_S_NOMATCH_CV  11779

/*** TLS_SORT_SN_LISTSHORT_CV: List to hold indices of keys too short.   */
/*   (Search_Near)   */
#define TLS_SORT_SN_LISTSHORT_CV  12034

/*** TLS_SORT_SN_NOMATCH_CV: Matching key not found in table. (Search   */
/*   _Near)   */
#define TLS_SORT_SN_NOMATCH_CV  12291

/*** TLS_SORT_SI_LISTSHORT_CV: Index array shorter than array to be sor   */
/*   ted (Index_Sort)   */
#define TLS_SORT_SI_LISTSHORT_CV  14082

/*** TLS_SORT_SNI_NOMATCH_CV: Key not found (Search_Near_Index)   */
/*      */
#define TLS_SORT_SNI_NOMATCH_CV  14339

/*** TLS_SORT_SNI_LISTSHORT_CV: Output array too small to hold all rows   */
/*   found               (Search_Near_Index)   */
#define TLS_SORT_SNI_LISTSHORT_CV  14594

/*** TLS_SORT_SI_NOMATCH_CV: Key not found in specified column of tab   */
/*   le (Search_Index)   */
#define TLS_SORT_SI_NOMATCH_CV  14851
