#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct id_list_all
    {
        int myorganid;
        int organrootid;
        int ownerid;
        int organparentid;
        int xx;
        int yy;
        char *ttype;
        char *organdirr;
        int last_organ_ent_id;
        int last_protein_ent_id;
    } id_list;

id_list last_organ_finder_for_first_root(id_list *my_list, int entity_count)
{
    id_list tmp;
    tmp.myorganid = 0;
    tmp.organrootid = 0;
    tmp.ownerid = 0;
    tmp.organparentid = 0;
    tmp.xx = 0;
    tmp.yy = 0;
    tmp.ttype = 0;
    tmp.organdirr = 0;
    tmp.last_organ_ent_id = 0;
    tmp.last_protein_ent_id = 0;

    int prior_id_flag = 0;
    entity_count -= 1;
    while (entity_count > 0)
    {
        if ((my_list[entity_count].myorganid != 0) && (my_list[entity_count].ownerid == 1) && (my_list[entity_count].organrootid == 1))
        {
            if (prior_id_flag < my_list[entity_count].myorganid)
            {
                prior_id_flag = my_list[entity_count].myorganid;
                my_list[entity_count].last_organ_ent_id = entity_count;
                tmp = my_list[entity_count];
            }
        }
        entity_count--;
    }
    return (tmp);
}

id_list last_organ_finder_for_second_root(id_list *my_list, int entity_count)
{
    id_list tmp;
    tmp.myorganid = 0;
    tmp.organrootid = 0;
    tmp.ownerid = 0;
    tmp.organparentid = 0;
    tmp.xx = 0;
    tmp.yy = 0;
    tmp.ttype = 0;
    tmp.organdirr = 0;
    tmp.last_organ_ent_id = 0;
    tmp.last_protein_ent_id = 0;

    int prior_id_flag = 0;
    entity_count -= 1;
    while (entity_count > 0)
    {
        if (my_list[entity_count].myorganid != 0 && my_list[entity_count].ownerid == 1 && my_list[entity_count].organrootid != 1)
        {
            if (prior_id_flag < my_list[entity_count].myorganid)
            {
                prior_id_flag = my_list[entity_count].myorganid;
                my_list[entity_count].last_organ_ent_id = entity_count;
                tmp = my_list[entity_count];
            }
        }
        entity_count--;
    }
    return (tmp);
}

id_list protein_information_logger(id_list *my_list, int entity_count)
{
    int cntr = 0;
    while (entity_count > cntr)
    {
        if (my_list[cntr].ttype[0] == 'A')
        {
            my_list[cntr].last_protein_ent_id = cntr;
            return (my_list[cntr]);
        }
        cntr++;
    }
    return (my_list[entity_count]);
}

int main()
{
    int width;     // columns in the game grid
    int height;     // rows in the game grid
    scanf("%d%d", &width, &height);
    char direction;
    
    int i;
    int move_count = 0;
    int mcst = 0;
    int mcnd = 0;
    int mcrd = 0;
    int harvester_flag = 0;
    id_list *last_organ_rootone = calloc(100, sizeof(id_list));
    id_list *last_organ_rootsecond = calloc(100, sizeof(id_list));
    id_list *protein_info = calloc(100, sizeof(id_list));
    int lastorg2id = 0;
    int lastorg2locx = 0;
    int lastorg2locy = 0;

    while (1) 
    {
        int entity_count;
        scanf("%d", &entity_count);
        i = 0; 
        id_list my_list[entity_count];

        while (i < entity_count) 
        {
            int x; // grid coordinate
            int y; // grid coordinate
            char type[162][33]; // WALL, ROOT, BASIC, TENTACLE, HARVESTER, SPORER, A, B, C, D
            int owner; // 1 if your organ, 0 if enemy organ, -1 if neither
            int organ_id; // id of this entity if it's an organ, 0 otherwise
            char organ_dir[162][2]; // N,E,S,W or X if not an organ
            int organ_parent_id;
            int organ_root_id;
            scanf("%d%d%s%d%d%s%d%d", &x, &y, type[i], &owner, &organ_id, organ_dir[i], &organ_parent_id, &organ_root_id);
            my_list[i].ownerid = owner;
            my_list[i].myorganid = organ_id;
            my_list[i].organparentid = organ_parent_id;
            my_list[i].organrootid = organ_root_id;
            my_list[i].organdirr = organ_dir[i];
            my_list[i].xx = x;
            my_list[i].yy = y;
            my_list[i].ttype = type[i];
            i++;
        }
        last_organ_rootsecond[move_count] = last_organ_finder_for_second_root(my_list, entity_count);
        protein_info[move_count] = protein_information_logger(my_list, entity_count);
        last_organ_rootone[move_count] = last_organ_finder_for_first_root(my_list, entity_count);
        
        int my_a;
        int my_b;
        int my_c;
        int my_d;
        scanf("%d%d%d%d", &my_a, &my_b, &my_c, &my_d);
        int opp_a;
        int opp_b;
        int opp_c;
        int opp_d;
        scanf("%d%d%d%d", &opp_a, &opp_b, &opp_c, &opp_d);
        int required_actions_count;
        scanf("%d", &required_actions_count);
        fprintf(stderr, "Ac C: %d\n", required_actions_count);
        for (int j = 0; j < required_actions_count; j++) 
        {
            if (mcst == 0)
            {
                if (protein_info[move_count].yy == 1)
                {
                    printf("GROW 1 1 1 SPORER E\n");
                }
                else if (protein_info[move_count].yy == 2)
                {
                    printf("GROW 1 2 2 SPORER E\n");
                }
                else if (protein_info[move_count].yy == 3)
                {
                    printf("GROW 1 1 3 SPORER E\n");
                }
                mcst++;
            }
            else if (mcst == 1)
            {
                printf("SPORE %d %d %d\n",last_organ_rootone[move_count].myorganid, protein_info[move_count].xx - 2, protein_info[move_count].yy);
                mcst++;
            }

            else if (mcnd == 0 && mcst > 1)
            {
                printf("GROW %d %d %d HARVESTER E\n", last_organ_rootsecond[move_count].myorganid, protein_info[move_count].xx - 1, protein_info[move_count].yy);
                mcnd++;
            }
            else if (mcnd == 1 && mcst > 1)
            {
                printf("WAIT\n");
                mcnd++;
            }
            else if (mcnd == 2 && mcst > 1 && mcrd == 0)
            {
                printf("GROW %d %d %d BASIC N\n", last_organ_rootone[move_count].myorganid, last_organ_rootone[move_count].xx + 1, last_organ_rootone[move_count].yy);
                mcrd++;
            }
            else
            {
                printf("WAIT\n");
                mcrd = 0;
            }
                
        }

        fprintf(stderr, "Entitiy Count: %d\n",entity_count);
        fprintf(stderr, "------------------------------------------\n");
        fprintf(stderr, "Root 1 Last Organ Entity id: %d\n",last_organ_rootone[move_count].last_organ_ent_id);
        fprintf(stderr, "Root 1 Last Organ X: %d\n",last_organ_rootone[move_count].xx);
        fprintf(stderr, "Root 1 Last Organ Y: %d\n",last_organ_rootone[move_count].yy);
        fprintf(stderr, "Root 1 Last Organ Type: %s\n",last_organ_rootone[move_count].ttype);   
        fprintf(stderr, "Root 1 Last Organ Owner: %d\n",last_organ_rootone[move_count].ownerid);
        fprintf(stderr, "Root 1 Last Organ ID: %d\n",last_organ_rootone[move_count].myorganid);   
        fprintf(stderr, "Root 1 Last Organ Dir: %s\n",last_organ_rootone[move_count].organdirr);   
        fprintf(stderr, "Root 1 Last Organ Parent ID: %d\n",last_organ_rootone[move_count].organparentid);   
        fprintf(stderr, "Root 1 Last Organ Root ID: %d\n",last_organ_rootone[move_count].organrootid);
        fprintf(stderr, "------------------------------------------\n");
        fprintf(stderr, "Root 2 Last Organ Entity id: %d\n",last_organ_rootsecond[move_count].last_organ_ent_id);
        fprintf(stderr, "Root 2 Last Organ X: %d\n",last_organ_rootsecond[move_count].xx);
        fprintf(stderr, "Root 2 Last Organ Y: %d\n",last_organ_rootsecond[move_count].yy);
        fprintf(stderr, "Root 2 Last Organ Type: %s\n",last_organ_rootsecond[move_count].ttype);   
        fprintf(stderr, "Root 2 Last Organ Owner: %d\n",last_organ_rootsecond[move_count].ownerid);
        fprintf(stderr, "Root 2 Last Organ ID: %d\n",last_organ_rootsecond[move_count].myorganid);   
        fprintf(stderr, "Root 2 Last Organ Dir: %s\n",last_organ_rootsecond[move_count].organdirr);   
        fprintf(stderr, "Root 2 Last Organ Parent ID: %d\n",last_organ_rootsecond[move_count].organparentid);   
        fprintf(stderr, "Root 2 Last Organ Root ID: %d\n",last_organ_rootsecond[move_count].organrootid);
        fprintf(stderr, "------------------------------------------\n");
        fprintf(stderr, "A Protein Entity id: %d\n",protein_info[move_count].last_protein_ent_id);
        fprintf(stderr, "A Protein X: %d\n",protein_info[move_count].xx);
        fprintf(stderr, "A Protein Y: %d\n",protein_info[move_count].yy);
        move_count++;
    }
    return (0);
}
