/* datatypes.h 
 * Prof. Russell 
 * harlanr
 * ECE 2230 Fall 2020 
 * MP2
 * 
 * Propose: The data type that is stored in the list ADT is defined here.  We
 *          define a single mapping that allows the list ADT to be defined in
 *          terms of a generic data_t.
 *
 * Assumptions: 
 *     For MP2 there are no changes to this file.  For all other MP's
 *     this file must be updated
 *
 *     data_t: The type of data that we want to store in the list
 *
 * Bugs:
 */

typedef struct alert_tag {
    int generator_id;   // ID of component generating alert
    int signature_id;   // ID of detection rule
    int revision_id;    // revision number of detection rule
    int dest_ip_addr;   // IP address of destination
    int src_ip_addr;    // IP address of source 
    int dest_port_num;  // port number at destination
    int src_port_num;   // port number at source host
    int timestamp;      // time in seconds alert received
} alert_t;

/* the list ADT works on alert data of this type */
typedef alert_t data_t;

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
