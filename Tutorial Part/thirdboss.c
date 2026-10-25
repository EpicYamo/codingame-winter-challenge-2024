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

id_list last_organ_finder(id_list *my_list, int entity_count)
{
    id_list tmp;
    int prior_id_flag = 0;
    entity_count -= 1;
    while (entity_count > 0)
    {
        if ((my_list[entity_count].myorganid != 0) && (my_list[entity_count].ownerid == 1))
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

int main()
{
    int width;     // columns in the game grid
    int height;     // rows in the game grid
    scanf("%d%d", &width, &height);
    char direction;
    
    int i;
    int move_count = 0;
    int move_flag_down = 0;
    int move_flag_up = 0;
    int move_flag_right = 0;
    int ss = 0;


    while (1) 
    {
        int entity_count;
        scanf("%d", &entity_count);
        i = 0; 
        id_list my_list[entity_count];
        id_list last_organ[200];
        id_list protein_info[100];

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
        last_organ[move_count] = last_organ_finder(my_list, entity_count);

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

        for (int j = 0; j < required_actions_count; j++) 
        {
            if (move_count < 3)
            {
                printf("GROW %d 1 %d TENTACLE E\n",last_organ[move_count].myorganid, 3 + ss);
                ss++;
            }
            else if (move_flag_right == 0)
            {
                printf("GROW %d %d %d TENTACLE E\n",last_organ[move_count].myorganid, last_organ[move_count].xx + 1, last_organ[move_count].yy);
                move_flag_right = 1;
            }
            else if (move_flag_up < 3)
            {
                printf("GROW %d %d %d TENTACLE E\n",last_organ[move_count].myorganid, last_organ[move_count].xx, last_organ[move_count].yy - 1);
                move_flag_up++;
                if (move_flag_up == 3)
                {
                    move_flag_right = 0;
                    move_flag_down = 0;
                }
            }
            else if (move_flag_down < 3)
            {
                printf("GROW %d %d %d TENTACLE E\n",last_organ[move_count].myorganid, last_organ[move_count].xx, last_organ[move_count].yy + 1);
                move_flag_down++;
                if (move_flag_down == 3)
                {
                    move_flag_right = 0;
                    move_flag_up = 0;
                }
            }
        }

        fprintf(stderr, "Entitiy Count: %d\n",entity_count);
        fprintf(stderr, "------------------------Seperator\n");
        fprintf(stderr, "Last Organ Entity id: %d\n",last_organ[move_count].last_organ_ent_id);
        fprintf(stderr, "Last Organ X: %d\n",last_organ[move_count].xx);
        fprintf(stderr, "Last Organ Y: %d\n",last_organ[move_count].yy);
        fprintf(stderr, "Last Organ Type: %s\n",last_organ[move_count].ttype);   
        fprintf(stderr, "Last Organ Owner: %d\n",last_organ[move_count].ownerid);
        fprintf(stderr, "Last Organ ID: %d\n",last_organ[move_count].myorganid);   
        fprintf(stderr, "Last Organ Dir: %s\n",last_organ[move_count].organdirr);   
        fprintf(stderr, "Last Organ Parent ID: %d\n",last_organ[move_count].organparentid);   
        fprintf(stderr, "Last Organ Root ID: %d\n",last_organ[move_count].organrootid);
        move_count++;
    }
    return (0);
}
