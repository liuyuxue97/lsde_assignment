#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

#include "khash.h"
#include "utils.h"

unsigned long person_length, knows_length;
unsigned long new_person_filter,new_knows_length;
unsigned long person_set_size=3000,knows_set_size=3000；

typedef struct
{
    unsigned long  person_id;
    unsigned short birthday;
    unsigned short know_each_other_first;
    unsigned short know_each_other_n;
    unsigned short interest_first;
    unsigned short interest_n;
}Person_Simple;

Person *person_map;
unsigned int *knows_map;

Person_Simple *new_person_map;
unsigned int *new_knows_map;

KHASH_MAP_INIT_INT64(pht, unsigned int)
khash_t(pht) *person_offsets;

FILE   *person_out;
FILE   *knows_out;

int main(int argc, char *argv[])
{
    unsigned long person_set_size;
    unsigned long knows_set_size=2000;
    
    unsigned int person_offset;
    unsigned long knows_offset, knows_offset2;
    Person *person, *knows;
    
    char* person_output_file   = makepath(argv[1], "person_re",   "bin");
    char* knows_output_file    = makepath(argv[1], "knows_re",    "bin");
    
    person_map   = (Person *)         mmapr(makepath(argv[1], "person",   "bin"), &person_length);
    knows_map    = (unsigned int *)   mmapr(makepath(argv[1], "knows",    "bin"), &knows_length);

    int ret;
    khiter_t k;
    person_offsets = kh_init(pht);
    
    person_set_size=person_length/sizeof(Person);
    new_person_map= malloc((person_set_size * sizeof (Person_Simple)));
    new_knows_map=malloc(knows_set_size* sizeof(unsigned int));

    for (person_offset = 0; person_offset < person_length/sizeof(Person); person_offset++)
    {
        int know_flag=0;
        person = &person_map[person_offset];
        unsigned short count_n=0;
        //printf("I am break point1\n");
        for(knows_offset=person->knows_first;knows_offset<person->knows_first+person->knows_n;knows_offset++)
        {
            knows=&person_map[knows_map[knows_offset]];
            // they must in the same city
            if(person->location!=knows->location)continue;
                    count_n++;
                    if(know_flag==0)
                    {
                        know_flag=1;
                        if(new_person_filter>=person_set_size)
                        {
                            person_set_size+=5000;
                            new_person_map=realloc(new_person_map,person_set_size* sizeof(Person_Simple));
                        }
                        new_person_map[new_person_filter].person_id=person->person_id;
                        new_person_map[new_person_filter].birthday=person->birthday;
                        new_person_map[new_person_filter].interest_first=new_interest_length;
                        new_person_map[new_person_filter].interest_n=person->interest_n;
                        new_person_map[new_person_filter].know_each_other_first=new_knows_length;
                    }
                    new_knows_map[new_knows_length]=knows_map[knows_offset];
                    new_knows_length++;
                    break;
        }

        if(know_flag!=0){
            new_person_map[new_person_filter].know_each_other_n=count_n;
            k = kh_put(pht, person_offsets, person_offset, &ret);
            kh_value(person_offsets, k) = new_person_filter;
            new_person_filter++;
        }
        for(knows_offset=0;knows_offset<new_knows_length;knows_offset++)
    {
        person_offset=kh_value(person_offsets,kh_get(pht,person_offsets,new_knows_map[knows_offset]));
        new_knows_map[knows_offset]=person_offset;
    }
    }   
    // save person_re.bin
    person_out=open_binout(person_output_file);
    fwrite(new_person_map, sizeof(Person_Simple),new_person_filter,person_out);
    //printf("I am break point4\n");
    knows_out=open_binout(knows_output_file);
    fwrite(new_knows_map, sizeof(unsigned int),new_knows_length,knows_out);

    return 0;
}