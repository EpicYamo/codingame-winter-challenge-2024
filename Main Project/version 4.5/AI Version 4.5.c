#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

typedef struct id_list_all
{
    int myorganid; // id of this entity if it's an organ, 0 otherwise
    int organrootid; // root id of the organ
    int ownerid; // 1 if your organ, 0 if enemy organ, -1 if neither
    int organparentid; // previous organ id of the current organ id
    int xx; // grid coordinate
    int yy; // grid coordinate
    char *ttype; // WALL, ROOT, BASIC, TENTACLE, HARVESTER, SPORER, A, B, C, D
    char *organdirr; // N,E,S,W or X if not an organ
} id_list;

typedef struct coordinates_s
{
    int x1;
    int y1;
} coordinates;

typedef struct layer_one_s
{
    coordinates up;
    coordinates down;
    coordinates right;
    coordinates left;
} layer_one;

typedef struct layer_two_s
{
    coordinates up_up_0;
    coordinates upper_right_cross_1;
    coordinates right_right_2;
    coordinates lower_right_cross_3;
    coordinates down_down_4;
    coordinates lower_left_cross_5;
    coordinates left_left_6;
    coordinates upper_left_cross_7;
} layer_two;

typedef struct move_id_list_s
{
    int validity; // 1 if its a possible move / 0 if its not
    int myorganidm; // id of this entity if it's an organ, 0 otherwise
    int organrootidm; // root id of the organ
    int owneridm; // 1 if your organ, 0 if enemy organ, -1 if neither
    int organparentidm; // previous organ id of the current organ id
    int xxm; // grid coordinate
    int yym; // grid coordinate
    char *ttypem; // WALL, ROOT, BASIC, TENTACLE, HARVESTER, SPORER, A, B, C, D , WALL = W , ROOT = R, TENTACLE = T, HARVESTER = H, SPORER = S, BASIC = G. 
    char *organdirrm; // N,E,S,W or X if not an organ
    char *move_loc; //     U   ,   D   ,      R   ,    L    ,   N          ,         O            ,           P           ,         I          ,         U
                    //   UP_0    DOWN_4    RIGHT_2   LEFT_6    NONE           upper_right_cross_1     lower_right_cross_3    lower_left_cross_5     upper_left_cross_7
                    // INFOS FOR LAYER 2. LAYER 1 = U:0 D:1 R:2 L:3
} move_id_list;

int             root_counter(id_list *my_list, int entity_count); // number of owned roots available in the map
int             protein_counter(id_list *my_list, int entity_count); // number of proteins available in the map
id_list         *last_organ_finder_per_root(id_list *my_list, int entity_count); //keeps the last organ for each root, each root are an index.
id_list         *protein_information_logger(id_list *my_list, int entity_count); //keeps all protein in the index.
id_list         *store_roots_func(id_list *my_list, int entity_count); // stores the roots as indexes.
void            ft_engine(id_list organ_root_id, id_list *my_list, int entity_count, int width, int height, int my_a, int my_b ,int my_c ,int my_d, id_list *protein_info);
layer_one       set_layer_one(id_list organ); // sets layer one 
layer_two       set_layer_two(layer_one layerone); // sets layer two
move_id_list    *check_validity_layerone(layer_one layerone, id_list *my_list, int entity_count, int width, int height); //checks valid possible moves of layer one
char            find_from_coordinates(id_list *my_list, int count_tmp, int x, int y); // finds what is in the coordinate (legacy).
move_id_list    *check_validity_layertwo(move_id_list *pos_mov_l1, layer_two layertwo, id_list *my_list, int entity_count, int width, int height); // checks valid possible moves of layer two according to the possible moves of layer one
id_list         find_nearest_protein(id_list organ, id_list *protein_info, char *typenext, id_list *skip_closest_protein, int j); // finds the nearest protein
float           calculateDistance(float x1, float y1, float x2, float y2); // calculates the distance
id_list         get_organ_parent(id_list *my_list, int entity_count, id_list organ); //returns organ parent
id_list         find_according_to_coordinates_id_list(id_list *my_list, int count_tmp, int x, int y); // finds what is in the coordinate (upgraded version).
id_list         *stack_my_organs(id_list *my_list, int count_tmp, id_list organ_root_id); //keeps my organs until this round uses nodes
int             my_organ_counter(id_list *my_list, int entity_count, id_list organ_root_id); // returns organ count per root
id_list         find_last_organ(id_list *my_list, int entity_count, id_list organ_root_id); // finds the last organ according to the root id in the map by checking every piece one by one
void            ft_priority_is_defense(int i_c_tmp, int *attacked, id_list *my_list, int entity_count, layer_two *layertwo, move_id_list **pos_mov_l1, int my_b, int my_c, id_list *organ_list);
int             ft_check_for_harvester_availability(id_list *my_list, int entity_count, char protein); // checks for harvester availability and also checks what kind of protein it harvests. if it matches with the given protein it returns 1.
void            go_to_protein_sacrifice(id_list *my_list, int entity_count, int width, int height, id_list *protein_info, char *typeee, int my_a, int my_b, int my_c, int my_d, id_list organ_root_id, int aggressive_switch); // go nearest protein upgraded version.
int             check_if_protein_is_harvested(move_id_list protein, id_list *my_list, int entity_count);

void ft_engine(id_list organ_root_id, id_list *my_list, int entity_count, int width, int height, int my_a, int my_b ,int my_c ,int my_d, id_list *protein_info)
{
    id_list         *organ_list = stack_my_organs(my_list, entity_count, organ_root_id);
    layer_one       layerone[500];
    layer_two       layertwo[500];
    move_id_list    *pos_mov_l1[500]; // layer one
    move_id_list    *pos_mov_l2[500]; // layer two
    id_list         organ_tmp;
    int i_oc = my_organ_counter(my_list, entity_count, organ_root_id) - 1;
    int i_c_tmp = i_oc;
    while (i_c_tmp >= 0)
    {
        organ_tmp  = organ_list[i_c_tmp];
        layerone[i_c_tmp] = set_layer_one(organ_tmp);
        layertwo[i_c_tmp] = set_layer_two(layerone[i_c_tmp]);
        pos_mov_l1[i_c_tmp] = check_validity_layerone(layerone[i_c_tmp], my_list, entity_count, width, height);
        pos_mov_l2[i_c_tmp] = check_validity_layertwo(pos_mov_l1[i_c_tmp], layertwo[i_c_tmp], my_list, entity_count, width, height);
        i_c_tmp -= 1;
    }
    char A_Protein[2];
    A_Protein[0] = 'A';
    A_Protein[1] = '\0';
    char B_Protein[2];
    B_Protein[0] = 'B';
    B_Protein[1] = '\0';
    char C_Protein[2];
    C_Protein[0] = 'C';
    C_Protein[1] = '\0';
    char D_Protein[2];
    D_Protein[0] = 'D';
    D_Protein[1] = '\0';
    int free_counter_l1;
    int free_counter_l2; 
    int move_checker;
    int harvester_a = ft_check_for_harvester_availability(my_list, entity_count, 'A');
    int harvester_b = ft_check_for_harvester_availability(my_list, entity_count, 'B');
    int harvester_c = ft_check_for_harvester_availability(my_list, entity_count, 'C');
    int harvester_d = ft_check_for_harvester_availability(my_list, entity_count, 'D');

    move_checker = 0;
    i_c_tmp = i_oc;
    // develop defense layer 3 only for up down right left 4 locations
    if (move_checker == 0)
        ft_priority_is_defense(i_c_tmp, &move_checker, my_list, entity_count, layertwo, pos_mov_l1, my_b, my_c, organ_list);
    
    if (harvester_a != 1 && move_checker == 0)
    {
        go_to_protein_sacrifice(my_list, entity_count, width, height, protein_info, A_Protein, my_a, my_b, my_c, my_d, organ_root_id, 1);
        move_checker += 1;
    }

    if (harvester_c != 1 && move_checker == 0)
    {
        go_to_protein_sacrifice(my_list, entity_count, width, height, protein_info, C_Protein, my_a, my_b, my_c, my_d, organ_root_id, 0);
        move_checker += 1;
    }

    if (harvester_d != 1 && move_checker == 0)
    {
        go_to_protein_sacrifice(my_list, entity_count, width, height, protein_info, D_Protein, my_a, my_b, my_c, my_d, organ_root_id, 0);
        move_checker += 1;
    }

    if (harvester_b != 1 && move_checker == 0)
    {
        go_to_protein_sacrifice(my_list, entity_count, width, height, protein_info, B_Protein, my_a, my_b, my_c, my_d, organ_root_id, 0);
        move_checker += 1;
    }
        
    if (move_checker == 0)
    {
        go_to_protein_sacrifice(my_list, entity_count, width, height, protein_info, A_Protein, my_a, my_b, my_c, my_d, organ_root_id, 0); // req functions :  attack to the enemy
        move_checker += 1;
    }

    i_c_tmp = i_oc;
    while (i_c_tmp >= 0)
    {
        free_counter_l1 = 0;
        free_counter_l2 = 0;
        while (free_counter_l2 < 8)
        {
            free(pos_mov_l2[i_c_tmp][free_counter_l2].move_loc);
            free_counter_l2 += 1;
        }
        while (free_counter_l1 < 4)
        {
            free(pos_mov_l1[i_c_tmp][free_counter_l1].move_loc);
            free_counter_l1 += 1;
        }
        free(pos_mov_l1[i_c_tmp]);
        free(pos_mov_l2[i_c_tmp]);
        i_c_tmp -= 1;
    }
    free(organ_list);
}

int main()
{
    int i; // important
    int width;     // columns in the game grid
    int height;     // rows in the game grid
    scanf("%d%d", &width, &height);

    while (1) 
    {
        int entity_count;
        scanf("%d", &entity_count);
        
        id_list my_list[entity_count];

        id_list *last_organ_per_root;
        id_list *protein_info;

        int root_count;
        int j = 0;

        i = 0; 
        while (i < entity_count) 
        {
            int x;
            int y;
            char type[500][33];
            int owner;
            int organ_id;
            char organ_dir[500][2];
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
        protein_info = protein_information_logger(my_list, entity_count);
        last_organ_per_root = last_organ_finder_per_root(my_list, entity_count);
        root_count = root_counter(my_list, entity_count);

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

        while (j < required_actions_count && root_count > 0) 
        {
            ft_engine(last_organ_per_root[root_count - 1], my_list, entity_count, width, height, my_a, my_b, my_c, my_d, protein_info);
            root_count--;
            j++;
        }
        free(last_organ_per_root);
        free(protein_info);
    }
    return (0);
}

id_list *store_roots_func(id_list *my_list, int entity_count)
{
    id_list *root_infos = calloc(200, sizeof(id_list));

    int root_count;
    root_count = root_counter(my_list, entity_count);

    int root_flag = 0;
    
    entity_count -= 1;
    while (entity_count >= 0 && root_count > 0)
    {
        while (root_flag == 0)
        {
            if ((my_list[entity_count].ownerid == 1) && (my_list[entity_count].ttype[0] == 'R'))
            {
                root_infos[root_count - 1] = my_list[entity_count];
                root_flag += 1;
            }
            entity_count--;
        }
        root_flag = 0;
        root_count -= 1;
    }
    return (root_infos);
}

int protein_counter(id_list *my_list, int entity_count)
{
    int protein_count = 0;
    
    entity_count -= 1;
    while (entity_count >= 0)
    {
        if (my_list[entity_count].ttype[0] == 'A' || my_list[entity_count].ttype[0] == 'B' || my_list[entity_count].ttype[0] == 'C'|| my_list[entity_count].ttype[0] == 'D')
        {
            if (my_list[entity_count].ttype[1] != 'A')
                protein_count++;
        }
        entity_count--;
    }
    return (protein_count);
}

int root_counter(id_list *my_list, int entity_count)
{
    int root_count = 0;

    entity_count -= 1;
    while (entity_count >= 0)
    {
        if ((my_list[entity_count].ownerid == 1) && (my_list[entity_count].ttype[0] == 'R'))
            root_count += 1;
        entity_count--;
    }
    return (root_count);
}

id_list *last_organ_finder_per_root(id_list *my_list, int entity_count)
{
    id_list *last_organ_per_root = calloc(200, sizeof(id_list));

    int root_count;
    root_count = root_counter(my_list, entity_count);

    id_list *root_infos;
    root_infos = store_roots_func(my_list, entity_count);

    int entc_tmp;
    int i = 0;

    int organ_id_flag;

    while (root_count > 0)
    {
        entc_tmp = entity_count - 1;
        organ_id_flag = 0;

        while (entc_tmp >= 0)
        {
            if ((my_list[entc_tmp].ownerid) == 1 && (organ_id_flag < my_list[entc_tmp].myorganid) && (my_list[entc_tmp].organrootid == root_infos[i].organrootid))
            {
                organ_id_flag = my_list[entc_tmp].myorganid;
                last_organ_per_root[i] = my_list[entc_tmp];
            }
            entc_tmp--;
        }
        i++;
        root_count--;
    }
    free (root_infos);
    return (last_organ_per_root);
}

id_list *protein_information_logger(id_list *my_list, int entity_count)
{
    int i;
    entity_count -= 1;
    id_list *protein_info = calloc(200, sizeof(id_list));

    i = 0;
    while (entity_count >= 0)
    {
        if (my_list[entity_count].ttype[0] == 'A' || my_list[entity_count].ttype[0] == 'B' || my_list[entity_count].ttype[0] == 'C'|| my_list[entity_count].ttype[0] == 'D')
        {
            if (my_list[entity_count].ttype[1] != 'A')
            {
                protein_info[i] = my_list[entity_count];  
                i++;
            }   
        }
        entity_count--;
    }
    return (protein_info);
}

layer_one set_layer_one(id_list organ)
{
    layer_one layerone;

    layerone.up.x1 = organ.xx;
    layerone.up.y1 = organ.yy - 1;
    layerone.down.x1 = organ.xx;
    layerone.down.y1 = organ.yy + 1;
    layerone.right.x1 = organ.xx + 1;
    layerone.right.y1 = organ.yy;
    layerone.left.x1 = organ.xx - 1;
    layerone.left.y1 = organ.yy;

    return (layerone);
}

layer_two set_layer_two(layer_one layerone)
{
    layer_two layertwo;

    layertwo.up_up_0.x1 = layerone.up.x1;
    layertwo.up_up_0.y1 = layerone.up.y1 - 1;

    layertwo.upper_right_cross_1.x1 = layerone.up.x1 + 1;
    layertwo.upper_right_cross_1.y1 = layerone.up.y1;

    layertwo.right_right_2.x1 = layerone.right.x1 + 1;
    layertwo.right_right_2.y1 = layerone.right.y1;

    layertwo.lower_right_cross_3.x1 = layerone.right.x1;
    layertwo.lower_right_cross_3.y1 = layerone.right.y1 + 1;

    layertwo.down_down_4.x1 = layerone.down.x1;
    layertwo.down_down_4.y1 = layerone.down.y1 + 1;

    layertwo.lower_left_cross_5.x1 = layerone.left.x1;
    layertwo.lower_left_cross_5.y1 = layerone.left.y1 + 1;

    layertwo.left_left_6.x1 = layerone.left.x1 - 1;
    layertwo.left_left_6.y1 = layerone.left.y1;

    layertwo.upper_left_cross_7.x1 = layerone.left.x1;
    layertwo.upper_left_cross_7.y1 = layerone.left.y1 - 1;

    return (layertwo);
}

move_id_list    *check_validity_layerone(layer_one layerone, id_list *my_list, int entity_count, int width, int height)
{
    move_id_list    *possible_moves = calloc(4, sizeof(move_id_list));
    int             count_tmp;

    char up[2];
    up[0] = '\0';
    up[1] = '\0';
    char down[2];
    down[0] = '\0';
    down[1] = '\0';
    char right[2];
    right[0] = '\0';
    right[1] = '\0';
    char left[2];
    left[0] = '\0';
    left[1] = '\0';

    count_tmp = entity_count - 1;

    up[0] = find_from_coordinates(my_list, count_tmp, layerone.up.x1, layerone.up.y1);
    down[0] = find_from_coordinates(my_list, count_tmp, layerone.down.x1, layerone.down.y1);
    right[0] = find_from_coordinates(my_list, count_tmp, layerone.right.x1, layerone.right.y1);
    left[0] = find_from_coordinates(my_list, count_tmp, layerone.left.x1, layerone.left.y1);

    possible_moves[0].validity = 0;
    possible_moves[1].validity = 0;
    possible_moves[2].validity = 0;
    possible_moves[3].validity = 0;

    
    if (up[0] == 'N' || up[0] == 'A' || up[0] == 'B' || up[0] == 'C' || up[0] == 'D')
    {
        if (!(layerone.up.x1 < 0 || layerone.up.y1 < 0 || layerone.up.x1 >= width || layerone.up.y1 >= height))
        {
            possible_moves[0].move_loc = calloc(2, sizeof(char));
            possible_moves[0].move_loc[0] = 'U';
            possible_moves[0].move_loc[1] = '\0';
            possible_moves[0].validity = 1;
            possible_moves[0].xxm = layerone.up.x1;
            possible_moves[0].yym = layerone.up.y1;
            possible_moves[0].ttypem = calloc(2, sizeof(char));
            possible_moves[0].ttypem[0] = up[0];
            possible_moves[0].ttypem[1] = up[1];
        }
    }
    if (possible_moves[0].validity == 0)
    {
        possible_moves[0].move_loc = calloc(2, sizeof(char));
        possible_moves[0].move_loc[0] = 'N';
        possible_moves[0].move_loc[1] = '\0';
        possible_moves[0].ttypem = calloc(2, sizeof(char));
        possible_moves[0].ttypem[0] = 'N';
        possible_moves[0].ttypem[1] = '\0';
    }

    if (down[0] == 'N' || down[0] == 'A' || down[0] == 'B' || down[0] == 'C' || down[0] == 'D')
    {
        if (!(layerone.down.x1 < 0 || layerone.down.y1 < 0 || layerone.down.x1 >= width || layerone.down.y1 >= height))
        {
            possible_moves[1].move_loc = calloc(2, sizeof(char));
            possible_moves[1].move_loc[0] = 'D';
            possible_moves[1].move_loc[1] = '\0';
            possible_moves[1].validity = 1;
            possible_moves[1].xxm = layerone.down.x1;
            possible_moves[1].yym = layerone.down.y1;
            possible_moves[1].ttypem = calloc(2, sizeof(char));
            possible_moves[1].ttypem[0] = down[0];
            possible_moves[1].ttypem[1] = down[1];
        }
    }
    if (possible_moves[1].validity == 0)
    {
        possible_moves[1].move_loc = calloc(2, sizeof(char));
        possible_moves[1].move_loc[0] = 'N';
        possible_moves[1].move_loc[1] = '\0';
        possible_moves[1].ttypem = calloc(2, sizeof(char));
        possible_moves[1].ttypem[0] = 'N';
        possible_moves[1].ttypem[1] = '\0';
    }

    if (right[0] == 'N' || right[0] == 'A' || right[0] == 'B' || right[0] == 'C' || right[0] == 'D')
    {
        if (!(layerone.right.x1 < 0 || layerone.right.y1 < 0 || layerone.right.x1 >= width || layerone.right.y1 >= height))
        {
            possible_moves[2].move_loc = calloc(2, sizeof(char));
            possible_moves[2].move_loc[0] = 'R';
            possible_moves[2].move_loc[1] = '\0';
            possible_moves[2].validity = 1;
            possible_moves[2].xxm = layerone.right.x1;
            possible_moves[2].yym = layerone.right.y1;
            possible_moves[2].ttypem = calloc(2, sizeof(char));
            possible_moves[2].ttypem[0] = right[0];
            possible_moves[2].ttypem[1] = right[1];
        }
    }
    if (possible_moves[2].validity == 0)
    {
        possible_moves[2].move_loc = calloc(2, sizeof(char));
        possible_moves[2].move_loc[0] = 'N';
        possible_moves[2].move_loc[1] = '\0';
        possible_moves[2].ttypem = calloc(2, sizeof(char));
        possible_moves[2].ttypem[0] = 'N';
        possible_moves[2].ttypem[1] = '\0';
    }

    if (left[0] == 'N' || left[0] == 'A' || left[0] == 'B' || left[0] == 'C' || left[0] == 'D')
    {
        if (!(layerone.left.x1 < 0 || layerone.left.y1 < 0 || layerone.left.x1 >= width || layerone.left.y1 >= height))
        {
            possible_moves[3].move_loc = calloc(2, sizeof(char));
            possible_moves[3].move_loc[0] = 'L';
            possible_moves[3].move_loc[1] = '\0';
            possible_moves[3].validity = 1;
            possible_moves[3].xxm = layerone.left.x1;
            possible_moves[3].yym = layerone.left.y1;
            possible_moves[3].ttypem = calloc(2, sizeof(char));
            possible_moves[3].ttypem[0] = left[0];
            possible_moves[3].ttypem[1] = left[1];
        }
    }
    if (possible_moves[3].validity == 0)
    {
        possible_moves[3].move_loc = calloc(2, sizeof(char));
        possible_moves[3].move_loc[0] = 'N';
        possible_moves[3].move_loc[1] = '\0';
        possible_moves[3].ttypem = calloc(2, sizeof(char));
        possible_moves[3].ttypem[0] = 'N';
        possible_moves[3].ttypem[1] = '\0';
    }

    return (possible_moves);
}

char    find_from_coordinates(id_list *my_list, int count_tmp, int x, int y)
{
    while (count_tmp >= 0)
    {
        if (my_list[count_tmp].xx == x && my_list[count_tmp].yy == y)
        {
            if (my_list[count_tmp].ttype[0] == 'W' || my_list[count_tmp].ttype[0] == 'R' || my_list[count_tmp].ttype[0] == 'T' || my_list[count_tmp].ttype[0] == 'H' || my_list[count_tmp].ttype[0] == 'S' || my_list[count_tmp].ttype[0] == 'A' || my_list[count_tmp].ttype[0] == 'B' || my_list[count_tmp].ttype[0] == 'C' || my_list[count_tmp].ttype[0] == 'D')
            {
                if (my_list[count_tmp].ttype[0] == 'B' && my_list[count_tmp].ttype[1] == 'A')
                    return ('G');
                else
                    return (my_list[count_tmp].ttype[0]);
            }        
        }
        count_tmp--;
    }
    return ('N');
}

move_id_list    *check_validity_layertwo(move_id_list *pos_mov_l1, layer_two layertwo, id_list *my_list, int entity_count, int width, int height)
{
    move_id_list    *pos_mov_l2 = calloc(8, sizeof(move_id_list));
    int             count_tmp;
    count_tmp = entity_count - 1;

    pos_mov_l2[0].validity = 0;
    pos_mov_l2[1].validity = 0;
    pos_mov_l2[2].validity = 0;
    pos_mov_l2[3].validity = 0;
    pos_mov_l2[4].validity = 0;
    pos_mov_l2[5].validity = 0;
    pos_mov_l2[6].validity = 0;
    pos_mov_l2[7].validity = 0;

    char up_up_0[2];
    up_up_0[0] = '\0';
    up_up_0[1] = '\0';

    char upper_right_cross_1[2];
    upper_right_cross_1[0] = '\0';
    upper_right_cross_1[1] = '\0';

    char right_right_2[2];
    right_right_2[0] = '\0';
    right_right_2[1] = '\0';

    char lower_right_cross_3[2];
    lower_right_cross_3[0] = '\0';
    lower_right_cross_3[1] = '\0';

    char down_down_4[2];
    down_down_4[0] = '\0';
    down_down_4[1] = '\0';

    char lower_left_cross_5[2];
    lower_left_cross_5[0] = '\0';
    lower_left_cross_5[1] = '\0';

    char left_left_6[2];
    left_left_6[0] = '\0';
    left_left_6[1] = '\0';

    char upper_left_cross_7[2];
    upper_left_cross_7[0] = '\0';
    upper_left_cross_7[1] = '\0';

    up_up_0[0] = find_from_coordinates(my_list, count_tmp, layertwo.up_up_0.x1, layertwo.up_up_0.y1);
    upper_right_cross_1[0] = find_from_coordinates(my_list, count_tmp, layertwo.upper_right_cross_1.x1, layertwo.upper_right_cross_1.y1);
    right_right_2[0] = find_from_coordinates(my_list, count_tmp, layertwo.right_right_2.x1, layertwo.right_right_2.y1);
    lower_right_cross_3[0] = find_from_coordinates(my_list, count_tmp, layertwo.lower_right_cross_3.x1, layertwo.lower_right_cross_3.y1);
    down_down_4[0] = find_from_coordinates(my_list, count_tmp, layertwo.down_down_4.x1, layertwo.down_down_4.y1);
    lower_left_cross_5[0] = find_from_coordinates(my_list, count_tmp, layertwo.lower_left_cross_5.x1, layertwo.lower_left_cross_5.y1);
    left_left_6[0] = find_from_coordinates(my_list, count_tmp, layertwo.left_left_6.x1, layertwo.left_left_6.y1);
    upper_left_cross_7[0] = find_from_coordinates(my_list, count_tmp, layertwo.upper_left_cross_7.x1, layertwo.upper_left_cross_7.y1);
    //     U   ,   D   ,      R   ,    L    ,   N          ,         O            ,           P           ,         I          ,         U
    //   UP_0    DOWN_4    RIGHT_2   LEFT_6    NONE           upper_right_cross_1     lower_right_cross_3    lower_left_cross_5     upper_left_cross_7
    if (pos_mov_l1[0].validity == 1)
    {
        if (up_up_0[0] == 'N' || up_up_0[0] == 'A' || up_up_0[0] == 'B' || up_up_0[0] == 'C' || up_up_0[0] == 'D')
        {
            if (!(layertwo.up_up_0.x1 < 0 || layertwo.up_up_0.y1 < 0 || layertwo.up_up_0.x1 >= width || layertwo.up_up_0.y1 >= height))
            {
                pos_mov_l2[0].move_loc = calloc(2, sizeof(char));
                pos_mov_l2[0].move_loc[0] = 'U';
                pos_mov_l2[0].move_loc[1] = '\0';
                pos_mov_l2[0].validity = 1;
                pos_mov_l2[0].xxm = layertwo.up_up_0.x1;
                pos_mov_l2[0].yym = layertwo.up_up_0.y1;
                pos_mov_l2[0].ttypem = calloc(2, sizeof(char));
                pos_mov_l2[0].ttypem[0] = up_up_0[0];
                pos_mov_l2[0].ttypem[1] = up_up_0[1];
            }
        }
    }
    if (pos_mov_l2[0].validity == 0)
    {
        pos_mov_l2[0].move_loc = calloc(2, sizeof(char));
        pos_mov_l2[0].move_loc[0] = 'N';
        pos_mov_l2[0].move_loc[1] = '\0';
        pos_mov_l2[0].ttypem = calloc(2, sizeof(char));
        pos_mov_l2[0].ttypem[0] = 'N';
        pos_mov_l2[0].ttypem[1] = '\0';
    }

    if (pos_mov_l1[0].validity == 1 || pos_mov_l1[2].validity == 1)
    {
        if (upper_right_cross_1[0] == 'N' || upper_right_cross_1[0] == 'A' || upper_right_cross_1[0] == 'B' || upper_right_cross_1[0] == 'C' || upper_right_cross_1[0] == 'D')
        {
            if (!(layertwo.upper_right_cross_1.x1 < 0 || layertwo.upper_right_cross_1.y1 < 0 || layertwo.upper_right_cross_1.x1 >= width || layertwo.upper_right_cross_1.y1 >= height))
            {
                pos_mov_l2[1].move_loc = calloc(2, sizeof(char));
                pos_mov_l2[1].move_loc[0] = 'O';
                pos_mov_l2[1].move_loc[1] = '\0';
                pos_mov_l2[1].validity = 1;
                pos_mov_l2[1].xxm = layertwo.upper_right_cross_1.x1;
                pos_mov_l2[1].yym = layertwo.upper_right_cross_1.y1;
                pos_mov_l2[1].ttypem = calloc(2, sizeof(char));
                pos_mov_l2[1].ttypem[0] = upper_right_cross_1[0];
                pos_mov_l2[1].ttypem[1] = upper_right_cross_1[1];
            }
        }
    }
    if (pos_mov_l2[1].validity == 0)
    {
        pos_mov_l2[1].move_loc = calloc(2, sizeof(char));
        pos_mov_l2[1].move_loc[0] = 'N';
        pos_mov_l2[1].move_loc[1] = '\0';
        pos_mov_l2[1].ttypem = calloc(2, sizeof(char));
        pos_mov_l2[1].ttypem[0] = 'N';
        pos_mov_l2[1].ttypem[1] = '\0';
    }

    if (pos_mov_l1[2].validity == 1)
    {
        if (right_right_2[0] == 'N' || right_right_2[0] == 'A' || right_right_2[0] == 'B' || right_right_2[0] == 'C' || right_right_2[0] == 'D')
        {
            if (!(layertwo.right_right_2.x1 < 0 || layertwo.right_right_2.y1 < 0 || layertwo.right_right_2.x1 >= width || layertwo.right_right_2.y1 >= height))
            {
                pos_mov_l2[2].move_loc = calloc(2, sizeof(char));
                pos_mov_l2[2].move_loc[0] = 'R';
                pos_mov_l2[2].move_loc[1] = '\0';
                pos_mov_l2[2].validity = 1;
                pos_mov_l2[2].xxm = layertwo.right_right_2.x1;
                pos_mov_l2[2].yym = layertwo.right_right_2.y1;
                pos_mov_l2[2].ttypem = calloc(2, sizeof(char));
                pos_mov_l2[2].ttypem[0] = right_right_2[0];
                pos_mov_l2[2].ttypem[1] = right_right_2[1];
            }
        }
    }
    if (pos_mov_l2[2].validity == 0)
    {
        pos_mov_l2[2].move_loc = calloc(2, sizeof(char));
        pos_mov_l2[2].move_loc[0] = 'N';
        pos_mov_l2[2].move_loc[1] = '\0';
        pos_mov_l2[2].ttypem = calloc(2, sizeof(char));
        pos_mov_l2[2].ttypem[0] = 'N';
        pos_mov_l2[2].ttypem[1] = '\0';
    }
    
    if (pos_mov_l1[2].validity == 1 || pos_mov_l1[1].validity == 1)
    {
        if (lower_right_cross_3[0] == 'N' || lower_right_cross_3[0] == 'A' || lower_right_cross_3[0] == 'B' || lower_right_cross_3[0] == 'C' || lower_right_cross_3[0] == 'D')
        {
            if (!(layertwo.lower_right_cross_3.x1 < 0 || layertwo.lower_right_cross_3.y1 < 0 || layertwo.lower_right_cross_3.x1 >= width || layertwo.lower_right_cross_3.y1 >= height))
            {
                pos_mov_l2[3].move_loc = calloc(2, sizeof(char));
                pos_mov_l2[3].move_loc[0] = 'P';
                pos_mov_l2[3].move_loc[1] = '\0';
                pos_mov_l2[3].validity = 1;
                pos_mov_l2[3].xxm = layertwo.lower_right_cross_3.x1;
                pos_mov_l2[3].yym = layertwo.lower_right_cross_3.y1;
                pos_mov_l2[3].ttypem = calloc(2, sizeof(char));
                pos_mov_l2[3].ttypem[0] = lower_right_cross_3[0];
                pos_mov_l2[3].ttypem[1] = lower_right_cross_3[1];
            }
        }
    }
    if (pos_mov_l2[3].validity == 0)
    {
        pos_mov_l2[3].move_loc = calloc(2, sizeof(char));
        pos_mov_l2[3].move_loc[0] = 'N';
        pos_mov_l2[3].move_loc[1] = '\0';
        pos_mov_l2[3].ttypem = calloc(2, sizeof(char));
        pos_mov_l2[3].ttypem[0] = 'N';
        pos_mov_l2[3].ttypem[1] = '\0';
    }

    if (pos_mov_l1[1].validity == 1)
    {
        if (down_down_4[0] == 'N' || down_down_4[0] == 'A' || down_down_4[0] == 'B' || down_down_4[0] == 'C' || down_down_4[0] == 'D')
        {
            if (!(layertwo.down_down_4.x1 < 0 || layertwo.down_down_4.y1 < 0 || layertwo.down_down_4.x1 >= width || layertwo.down_down_4.y1 >= height))
            {
                pos_mov_l2[4].move_loc = calloc(2, sizeof(char));
                pos_mov_l2[4].move_loc[0] = 'D';
                pos_mov_l2[4].move_loc[1] = '\0';
                pos_mov_l2[4].validity = 1;
                pos_mov_l2[4].xxm = layertwo.down_down_4.x1;
                pos_mov_l2[4].yym = layertwo.down_down_4.y1;
                pos_mov_l2[4].ttypem = calloc(2, sizeof(char));
                pos_mov_l2[4].ttypem[0] = down_down_4[0];
                pos_mov_l2[4].ttypem[1] = down_down_4[1];
            }
        }
    }
    if (pos_mov_l2[4].validity == 0)
    {
        pos_mov_l2[4].move_loc = calloc(2, sizeof(char));
        pos_mov_l2[4].move_loc[0] = 'N';
        pos_mov_l2[4].move_loc[1] = '\0';
        pos_mov_l2[4].ttypem = calloc(2, sizeof(char));
        pos_mov_l2[4].ttypem[0] = 'N';
        pos_mov_l2[4].ttypem[1] = '\0';
    }

    if (pos_mov_l1[1].validity == 1 || pos_mov_l1[3].validity == 1)
    {
        if (lower_left_cross_5[0] == 'N' || lower_left_cross_5[0] == 'A' || lower_left_cross_5[0] == 'B' || lower_left_cross_5[0] == 'C' || lower_left_cross_5[0] == 'D')
        {
            if (!(layertwo.lower_left_cross_5.x1 < 0 || layertwo.lower_left_cross_5.y1 < 0 || layertwo.lower_left_cross_5.x1 >= width || layertwo.lower_left_cross_5.y1 >= height))
            {
                pos_mov_l2[5].move_loc = calloc(2, sizeof(char));
                pos_mov_l2[5].move_loc[0] = 'I';
                pos_mov_l2[5].move_loc[1] = '\0';
                pos_mov_l2[5].validity = 1;
                pos_mov_l2[5].xxm = layertwo.lower_left_cross_5.x1;
                pos_mov_l2[5].yym = layertwo.lower_left_cross_5.y1;
                pos_mov_l2[5].ttypem = calloc(2, sizeof(char));
                pos_mov_l2[5].ttypem[0] = lower_left_cross_5[0];
                pos_mov_l2[5].ttypem[1] = lower_left_cross_5[1];
            }
        }
    }
    if (pos_mov_l2[5].validity == 0)
    {
        pos_mov_l2[5].move_loc = calloc(2, sizeof(char));
        pos_mov_l2[5].move_loc[0] = 'N';
        pos_mov_l2[5].move_loc[1] = '\0';
        pos_mov_l2[5].ttypem = calloc(2, sizeof(char));
        pos_mov_l2[5].ttypem[0] = 'N';
        pos_mov_l2[5].ttypem[1] = '\0';
    }

    if (pos_mov_l1[3].validity == 1)
    {
        if (left_left_6[0] == 'N' || left_left_6[0] == 'A' || left_left_6[0] == 'B' || left_left_6[0] == 'C' || left_left_6[0] == 'D')
        {
            if (!(layertwo.left_left_6.x1 < 0 || layertwo.left_left_6.y1 < 0 || layertwo.left_left_6.x1 >= width || layertwo.left_left_6.y1 >= height))
            {
                pos_mov_l2[6].move_loc = calloc(2, sizeof(char));
                pos_mov_l2[6].move_loc[0] = 'L';
                pos_mov_l2[6].move_loc[1] = '\0';
                pos_mov_l2[6].validity = 1;
                pos_mov_l2[6].xxm = layertwo.left_left_6.x1;
                pos_mov_l2[6].yym = layertwo.left_left_6.y1;
                pos_mov_l2[6].ttypem = calloc(2, sizeof(char));
                pos_mov_l2[6].ttypem[0] = left_left_6[0];
                pos_mov_l2[6].ttypem[1] = left_left_6[1];
            }
        }
    }
    if (pos_mov_l2[6].validity == 0)
    {
        pos_mov_l2[6].move_loc = calloc(2, sizeof(char));
        pos_mov_l2[6].move_loc[0] = 'N';
        pos_mov_l2[6].move_loc[1] = '\0';
        pos_mov_l2[6].ttypem = calloc(2, sizeof(char));
        pos_mov_l2[6].ttypem[0] = 'N';
        pos_mov_l2[6].ttypem[1] = '\0';
    }

    if (pos_mov_l1[0].validity == 1 || pos_mov_l1[3].validity == 1)
    {
        if (upper_left_cross_7[0] == 'N' || upper_left_cross_7[0] == 'A' || upper_left_cross_7[0] == 'B' || upper_left_cross_7[0] == 'C' || upper_left_cross_7[0] == 'D')
        {
            if (!(layertwo.upper_left_cross_7.x1 < 0 || layertwo.upper_left_cross_7.y1 < 0 || layertwo.upper_left_cross_7.x1 >= width || layertwo.upper_left_cross_7.y1 >= height))
            {
                pos_mov_l2[7].move_loc = calloc(2, sizeof(char));
                pos_mov_l2[7].move_loc[0] = 'U';
                pos_mov_l2[7].move_loc[1] = '\0';
                pos_mov_l2[7].validity = 1;
                pos_mov_l2[7].xxm = layertwo.upper_left_cross_7.x1;
                pos_mov_l2[7].yym = layertwo.upper_left_cross_7.y1;
                pos_mov_l2[7].ttypem = calloc(2, sizeof(char));
                pos_mov_l2[7].ttypem[0] = upper_left_cross_7[0];
                pos_mov_l2[7].ttypem[1] = upper_left_cross_7[1];
            }
        }
    }
    if (pos_mov_l2[7].validity == 0)
    {
        pos_mov_l2[7].move_loc = calloc(2, sizeof(char));
        pos_mov_l2[7].move_loc[0] = 'N';
        pos_mov_l2[7].move_loc[1] = '\0';
        pos_mov_l2[7].ttypem = calloc(2, sizeof(char));
        pos_mov_l2[7].ttypem[0] = 'N';
        pos_mov_l2[7].ttypem[1] = '\0';
    }

    return (pos_mov_l2);
}

id_list find_nearest_protein(id_list organ, id_list *protein_info, char *typenext, id_list *skip_closest_protein, int j)
{
    coordinates start_loc;
    start_loc.x1 = organ.xx;
    start_loc.y1 = organ.yy;
    id_list return_protein;
    return_protein.myorganid = -2;
    return_protein.organdirr = 0;
    return_protein.organparentid = -2;
    return_protein.organrootid = -2;
    return_protein.ownerid = -2;
    return_protein.ttype = 0;
    return_protein.xx = -2;
    return_protein.yy = -2;
    if (protein_info[0].ttype == 0)
        return (return_protein);

    int tmp;
    int i = 0;
    float current_distance = 0;
    float previous_distance = -1;

    int counter;
    if (j > 0)
    {
        tmp = j - 1;
        while (tmp >= 0)
        {
            counter = 0;
            while (protein_info[counter].ttype != 0)
            {
                if ((protein_info[counter].xx == skip_closest_protein[tmp].xx) && (protein_info[counter].yy == skip_closest_protein[tmp].yy))
                {
                    protein_info[counter].ttype[0] = 'N';
                }
                counter++;
            }
            tmp--;
        }
    }

    while (protein_info[i].ttype != 0)
    {
        current_distance = calculateDistance(start_loc.x1, start_loc.y1, protein_info[i].xx, protein_info[i].yy);
        if ((previous_distance < 0 || current_distance < previous_distance) && (protein_info[i].ttype[0] == typenext[0]))
        {    
            previous_distance = current_distance;
            return_protein = protein_info[i];
        }
        i++;
    }
    return (return_protein);
}

float calculateDistance(float x1, float y1, float x2, float y2) 
{
    return (sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2)));
}

id_list get_organ_parent(id_list *my_list, int entity_count, id_list organ)
{
    id_list prev_organ;
    prev_organ = organ;
    entity_count -= 1;
    while(entity_count >= 0)
    {
        if (my_list[entity_count].myorganid == organ.organparentid)
        {
            prev_organ = my_list[entity_count];
        }
        entity_count--;
    }
    return (prev_organ);
}

id_list find_according_to_coordinates_id_list(id_list *my_list, int count_tmp, int x, int y)
{
    id_list error;
    error.myorganid = -2;
    error.organdirr = NULL;
    error.organparentid = -2;
    error.organrootid = -2;
    error.ownerid = -2;
    error.ttype = NULL;
    error.xx = -2;
    error.yy = -2;
    while (count_tmp >= 0)
    {
        if (my_list[count_tmp].xx == x && my_list[count_tmp].yy == y)
        {
            error = my_list[count_tmp];
        }
        count_tmp--;
    }
    return (error);
}

id_list find_last_organ(id_list *my_list, int entity_count, id_list organ_root_id)
{
    id_list the_last_organ;
    the_last_organ.myorganid = -2;
    the_last_organ.organdirr = NULL;
    the_last_organ.organparentid = -2;
    the_last_organ.organrootid = -2;
    the_last_organ.ownerid = -2;
    the_last_organ.ttype = NULL;
    the_last_organ.xx = -2;
    the_last_organ.yy = -2;
    int root_match = organ_root_id.organrootid;
    int count_tmp = entity_count - 1;
    int bigger_id_flag = 0;
    while (count_tmp >= 0)
    {
        if (my_list[count_tmp].ownerid == 1 && my_list[count_tmp].organrootid == root_match)
        {
            if(the_last_organ.myorganid < my_list[count_tmp].myorganid)
                the_last_organ = my_list[count_tmp];
        }
        count_tmp -= 1;
    }
    return (the_last_organ);
}

int my_organ_counter(id_list *my_list, int entity_count, id_list organ_root_id)
{
    int count_tmp = entity_count -1;
    int my_organ_count = 0;
    int root_match = organ_root_id.organrootid;
    while(count_tmp >= 0)
    {
        if(my_list[count_tmp].ownerid == 1 && my_list[count_tmp].organrootid == root_match)
            my_organ_count += 1;
        count_tmp -= 1;
    }
    return (my_organ_count);
}

id_list *stack_my_organs(id_list *my_list, int entity_count, id_list organ_root_id)
{
    id_list *organ_list = calloc(sizeof(id_list), 500);
    int     my_organ_count = my_organ_counter(my_list, entity_count, organ_root_id);
    int root_match = organ_root_id.organrootid;
    id_list the_last_organ; 
    int     count_tmp;
    int new_difference;
    int current_difference;
    int i;
    int j;

    i = 0;
    j = 1;
    the_last_organ = find_last_organ(my_list, entity_count, organ_root_id);
    organ_list[0] = the_last_organ;
    while (my_organ_count > 1)
    {
        current_difference = 1000000;
        new_difference = -1000000;
        count_tmp = entity_count -1;

        while (count_tmp >= 0)
        {
            if ((my_list[count_tmp].ownerid == 1) && (my_list[count_tmp].myorganid < organ_list[i].myorganid) && (my_list[count_tmp].organrootid == root_match))
            {
                new_difference = organ_list[i].myorganid - my_list[count_tmp].myorganid;
                if (new_difference < current_difference)
                {
                    organ_list[j] = my_list[count_tmp];
                    current_difference = organ_list[i].myorganid - organ_list[j].myorganid;
                }
            }
            count_tmp--;
        }

        j += 1;
        i += 1;
        my_organ_count -= 1;
    }
    return (organ_list);
}

void    ft_priority_is_defense(int i_c_tmp, int *attacked, id_list *my_list, int entity_count, layer_two *layertwo, move_id_list **pos_mov_l1, int my_b, int my_c, id_list *organ_list)
{
    id_list enemy_organ;
    int two_steps_ahead_counter;

    while(i_c_tmp >= 0 && *attacked == 0 && my_b != 0 && my_c != 0)
    {
        two_steps_ahead_counter = 0;
        while (two_steps_ahead_counter < 8 && *attacked == 0 && my_b != 0 && my_c != 0)
        {
            if (two_steps_ahead_counter == 0)
                enemy_organ = find_according_to_coordinates_id_list(my_list, entity_count -1, layertwo[i_c_tmp].up_up_0.x1, layertwo[i_c_tmp].up_up_0.y1);
            if (two_steps_ahead_counter == 1)
                enemy_organ = find_according_to_coordinates_id_list(my_list, entity_count -1, layertwo[i_c_tmp].upper_right_cross_1.x1, layertwo[i_c_tmp].upper_right_cross_1.y1);
            if (two_steps_ahead_counter == 2)
                enemy_organ = find_according_to_coordinates_id_list(my_list, entity_count -1, layertwo[i_c_tmp].right_right_2.x1, layertwo[i_c_tmp].right_right_2.y1);
            if (two_steps_ahead_counter == 3)
                enemy_organ = find_according_to_coordinates_id_list(my_list, entity_count -1, layertwo[i_c_tmp].lower_right_cross_3.x1, layertwo[i_c_tmp].lower_right_cross_3.y1);
            if (two_steps_ahead_counter == 4)
                enemy_organ = find_according_to_coordinates_id_list(my_list, entity_count -1, layertwo[i_c_tmp].down_down_4.x1, layertwo[i_c_tmp].down_down_4.y1);
            if (two_steps_ahead_counter == 5)
                enemy_organ = find_according_to_coordinates_id_list(my_list, entity_count -1, layertwo[i_c_tmp].lower_left_cross_5.x1, layertwo[i_c_tmp].lower_left_cross_5.y1);
            if (two_steps_ahead_counter == 6)
                enemy_organ = find_according_to_coordinates_id_list(my_list, entity_count -1, layertwo[i_c_tmp].left_left_6.x1, layertwo[i_c_tmp].left_left_6.y1);
            if (two_steps_ahead_counter == 7)
                enemy_organ = find_according_to_coordinates_id_list(my_list, entity_count -1, layertwo[i_c_tmp].upper_left_cross_7.x1, layertwo[i_c_tmp].upper_left_cross_7.y1);
            if (enemy_organ.ownerid == 0)
            {
                if (two_steps_ahead_counter == 0 && *attacked == 0)
                {
                    if (pos_mov_l1[i_c_tmp][0].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE N\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][0].xxm, pos_mov_l1[i_c_tmp][0].yym);
                        *attacked += 1;
                    }
                }
                if (two_steps_ahead_counter == 1 && *attacked == 0)
                {
                    if (pos_mov_l1[i_c_tmp][0].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE E\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][0].xxm, pos_mov_l1[i_c_tmp][0].yym);
                        *attacked += 1;
                    }
                    else if (pos_mov_l1[i_c_tmp][2].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE N\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][2].xxm, pos_mov_l1[i_c_tmp][2].yym);
                        *attacked += 1;
                    }
                }
                if (two_steps_ahead_counter == 2 && *attacked == 0)
                {
                    if (pos_mov_l1[i_c_tmp][2].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE E\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][2].xxm, pos_mov_l1[i_c_tmp][2].yym);
                        *attacked += 1;
                    }
                }
                if (two_steps_ahead_counter == 3 && *attacked == 0)
                {
                    if (pos_mov_l1[i_c_tmp][2].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE S\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][2].xxm, pos_mov_l1[i_c_tmp][2].yym);
                        *attacked += 1;
                    }
                    else if (pos_mov_l1[i_c_tmp][1].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE E\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][1].xxm, pos_mov_l1[i_c_tmp][1].yym);
                        *attacked += 1;
                    }
                }
                if (two_steps_ahead_counter == 4 && *attacked == 0)
                {
                    if (pos_mov_l1[i_c_tmp][1].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE S\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][1].xxm, pos_mov_l1[i_c_tmp][1].yym);
                        *attacked += 1;
                    }
                }
                if (two_steps_ahead_counter == 5 && *attacked == 0)
                {
                    if (pos_mov_l1[i_c_tmp][1].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE W\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][1].xxm, pos_mov_l1[i_c_tmp][1].yym);
                        *attacked += 1;
                    }
                    else if (pos_mov_l1[i_c_tmp][3].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE S\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][3].xxm, pos_mov_l1[i_c_tmp][3].yym);
                        *attacked += 1;
                    }
                }
                if (two_steps_ahead_counter == 6 && *attacked == 0)
                {
                    if (pos_mov_l1[i_c_tmp][3].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE W\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][3].xxm, pos_mov_l1[i_c_tmp][3].yym);
                        *attacked += 1;
                    }
                }
                if (two_steps_ahead_counter == 7 && *attacked == 0)
                {
                    if (pos_mov_l1[i_c_tmp][3].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE N\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][3].xxm, pos_mov_l1[i_c_tmp][3].yym);
                        *attacked += 1;
                    }
                    else if (pos_mov_l1[i_c_tmp][0].validity == 1)
                    {
                        printf("GROW %d %d %d TENTACLE W\n", organ_list[i_c_tmp].myorganid, pos_mov_l1[i_c_tmp][0].xxm, pos_mov_l1[i_c_tmp][0].yym);
                        *attacked += 1;
                    }
                }
            }
            two_steps_ahead_counter += 1;
        }
        i_c_tmp -= 1;
    }
}

int ft_check_for_harvester_availability(id_list *my_list, int entity_count, char protein)
{
    int count_tmp = entity_count - 1;
    while(count_tmp >= 0)
    {
        id_list harvesting_protein_info;
        harvesting_protein_info.myorganid = -2;
        harvesting_protein_info.organdirr = NULL;
        harvesting_protein_info.organparentid = -2;
        harvesting_protein_info.organrootid = -2;
        harvesting_protein_info.ownerid = -2;
        harvesting_protein_info.ttype = NULL;
        harvesting_protein_info.xx = -2;
        harvesting_protein_info.yy = -2;

        if(my_list[count_tmp].ownerid == 1 && my_list[count_tmp].ttype[0] == 'H')
        {
            if (my_list[count_tmp].organdirr[0] == 'N')
            {
                harvesting_protein_info = find_according_to_coordinates_id_list(my_list, entity_count -1, my_list[count_tmp].xx, my_list[count_tmp].yy - 1);
                if (harvesting_protein_info.xx == -2)
                    return (0);
                else if (harvesting_protein_info.ttype[0] == 'B' && harvesting_protein_info.ttype[1] != 'A' && protein == 'B')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'A' && protein == 'A')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'C' && protein == 'C')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'D' && protein == 'D')
                    return (1);
            }
            else if (my_list[count_tmp].organdirr[0] == 'S')
            {
                harvesting_protein_info = find_according_to_coordinates_id_list(my_list, entity_count -1, my_list[count_tmp].xx, my_list[count_tmp].yy + 1);
                if (harvesting_protein_info.xx == -2)
                    return (0);
                if (harvesting_protein_info.ttype[0] == 'B' && harvesting_protein_info.ttype[1] != 'A' && protein == 'B')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'A' && protein == 'A')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'C' && protein == 'C')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'D' && protein == 'D')
                    return (1);
            }
            else if (my_list[count_tmp].organdirr[0] == 'E')
            {
                harvesting_protein_info = find_according_to_coordinates_id_list(my_list, entity_count -1, my_list[count_tmp].xx + 1, my_list[count_tmp].yy);
                if (harvesting_protein_info.xx == -2)
                    return (0);
                if (harvesting_protein_info.ttype[0] == 'B' && harvesting_protein_info.ttype[1] != 'A' && protein == 'B')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'A' && protein == 'A')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'C' && protein == 'C')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'D' && protein == 'D')
                    return (1);
            }
            else if (my_list[count_tmp].organdirr[0] == 'W')
            {
                harvesting_protein_info = find_according_to_coordinates_id_list(my_list, entity_count -1, my_list[count_tmp].xx - 1, my_list[count_tmp].yy);
                if (harvesting_protein_info.xx == -2)
                    return (0);
                if (harvesting_protein_info.ttype[0] == 'B' && harvesting_protein_info.ttype[1] != 'A' && protein == 'B')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'A' && protein == 'A')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'C' && protein == 'C')
                    return (1);
                else if (harvesting_protein_info.ttype[0] == 'D' && protein == 'D')
                    return (1);
            }
        }
        count_tmp -=1;
    }
    return (0);
}

void go_to_protein_sacrifice(id_list *my_list, int entity_count, int width, int height, id_list *protein_info, char *typeee, int my_a, int my_b, int my_c, int my_d, id_list organ_root_id, int aggressive_switch)
{
    id_list         *organ_list = stack_my_organs(my_list, entity_count, organ_root_id);
    layer_one       layerone[500];
    layer_two       layertwo[500];
    move_id_list    *pos_mov_l1[500]; // layer one
    move_id_list    *pos_mov_l2[500]; // layer two
    id_list         organ_tmp;
    int i_oc = my_organ_counter(my_list, entity_count, organ_root_id) - 1;
    int i_c_tmp = i_oc;
    while (i_c_tmp >= 0)
    {
        organ_tmp  = organ_list[i_c_tmp];
        layerone[i_c_tmp] = set_layer_one(organ_tmp);
        layertwo[i_c_tmp] = set_layer_two(layerone[i_c_tmp]);
        pos_mov_l1[i_c_tmp] = check_validity_layerone(layerone[i_c_tmp], my_list, entity_count, width, height);
        pos_mov_l2[i_c_tmp] = check_validity_layertwo(pos_mov_l1[i_c_tmp], layertwo[i_c_tmp], my_list, entity_count, width, height);
        i_c_tmp -= 1;
    }
    int i;
    float distance_to_protein = 0;
    float distance_cmp = FLT_MAX;
    move_id_list chosen_move;
    chosen_move.move_loc = 0;
    chosen_move.myorganidm = 0;
    chosen_move.organdirrm = NULL;
    chosen_move.organparentidm = 0;
    chosen_move.organrootidm = 0;
    chosen_move.owneridm = 0;
    chosen_move.ttypem = NULL;
    chosen_move.validity = 0;
    chosen_move.xxm = 0;
    chosen_move.yym = 0;
    int free_counter_l1;
    int free_counter_l2;
    static id_list skip_closest_protein[200];
    id_list nearest_protein;
    static int j = 0;
    char rotation;
    int h;

    int move_made = 0;
    i_c_tmp = i_oc;
    int c = 0;
    nearest_protein = find_nearest_protein(organ_list[c], protein_info, typeee, skip_closest_protein, j);
    int harvester_a = ft_check_for_harvester_availability(my_list, entity_count, 'A');
    int harvester_b = ft_check_for_harvester_availability(my_list, entity_count, 'B');
    int harvester_c = ft_check_for_harvester_availability(my_list, entity_count, 'C');
    int harvester_d = ft_check_for_harvester_availability(my_list, entity_count, 'D');
    int harvested_or_not;
    if (nearest_protein.xx == -2)
    {
        printf("WAIT\n");
        move_made += 1;
    }
    i = 0;
	if (aggressive_switch == 1)
	{
		while (i < 4)
    	{ 
        	if (pos_mov_l1[c][i].validity == 1 && pos_mov_l1[c][i].ttypem[0] != typeee[0])
        	{
        	    distance_to_protein = calculateDistance(pos_mov_l1[c][i].xxm, pos_mov_l1[c][i].yym, nearest_protein.xx, nearest_protein.yy);
        	    if (distance_to_protein < distance_cmp || distance_cmp == FLT_MAX)
        	    {
        	        if (!(fabsf(distance_to_protein - distance_cmp) < 1e-6))
        	        {
        	            distance_cmp = distance_to_protein;
        	            chosen_move = pos_mov_l1[c][i];
        	        }   
        	    }
        	}
        	i++;
    	}
	}
	if (aggressive_switch == 0)
	{
		while (i < 4)
    	{ 
        	if (pos_mov_l1[c][i].validity == 1 && pos_mov_l1[c][i].ttypem[0] != 'A' && pos_mov_l1[c][i].ttypem[0] != 'B' && pos_mov_l1[c][i].ttypem[0] != 'C' && pos_mov_l1[c][i].ttypem[0] != 'D')
        	{
        	    distance_to_protein = calculateDistance(pos_mov_l1[c][i].xxm, pos_mov_l1[c][i].yym, nearest_protein.xx, nearest_protein.yy);
        	    if (distance_to_protein < distance_cmp || distance_cmp == FLT_MAX)
        	    {
        	        if (!(fabsf(distance_to_protein - distance_cmp) < 1e-6))
        	        {
        	            distance_cmp = distance_to_protein;
        	            chosen_move = pos_mov_l1[c][i];
        	        }   
        	    }
        	}
        	i++;
    	}
	}
    if (chosen_move.validity == 1)
    {
        if (((chosen_move.xxm - nearest_protein.xx < -1 || chosen_move.xxm - nearest_protein.xx > 1) || (chosen_move.yym - nearest_protein.yy < -1 || chosen_move.yym - nearest_protein.yy > 1)) || ((chosen_move.xxm - nearest_protein.xx == -1 || chosen_move.xxm - nearest_protein.xx == 1) && (chosen_move.yym - nearest_protein.yy == -1 || chosen_move.yym - nearest_protein.yy == 1)))
        {
            if (my_a > 0)
                printf("GROW %d %d %d BASIC N\n", organ_list[c].myorganid, chosen_move.xxm, chosen_move.yym);
            else if (my_b > 0 && my_c > 3)
                printf("GROW %d %d %d TENTACLE N\n", organ_list[c].myorganid, chosen_move.xxm, chosen_move.yym);
            else if (my_b > 0 && my_d > 3)
                printf("GROW %d %d %d SPORER N\n", organ_list[c].myorganid, chosen_move.xxm, chosen_move.yym);
            else if (my_d > 0 && my_c > 0)
                printf("GROW %d %d %d HARVESTER N\n", organ_list[c].myorganid, chosen_move.xxm, chosen_move.yym);
            else
                printf("GROW %d %d %d BASIC N\n", organ_list[c].myorganid, chosen_move.xxm, chosen_move.yym);
        }
        else if (((chosen_move.xxm - nearest_protein.xx == -1 || chosen_move.xxm - nearest_protein.xx == 1) && (chosen_move.yym == nearest_protein.yy)) || ((chosen_move.yym - nearest_protein.yy == -1 || chosen_move.yym - nearest_protein.yy == 1) && (chosen_move.xxm == nearest_protein.xx)))
        {
            // N yukarı, S Aşağı , W Sol, E Sağ.
            if (chosen_move.yym > nearest_protein.yy)
                rotation = 'N';
            else if (chosen_move.yym < nearest_protein.yy)
                rotation = 'S';
            else if (chosen_move.xxm > nearest_protein.xx)
                rotation = 'W';
            else if (chosen_move.xxm < nearest_protein.xx)
                rotation = 'E';
            printf("GROW %d %d %d HARVESTER %c\n", organ_list[c].myorganid, chosen_move.xxm, chosen_move.yym, rotation);
            skip_closest_protein[j] = nearest_protein;
            j += 1;
        }
        move_made += 1;
    }
    i_c_tmp = i_oc;
    c = 0;
    if (move_made == 0)
    {
        while(c <= i_c_tmp && move_made == 0)
        {
            h = 0;
            while (h < 4 && move_made == 0)
            {
                harvested_or_not = 0;
                if (pos_mov_l1[c][h].validity == 1)
                {
                    if ((pos_mov_l1[c][h].ttypem[0] == 'A') || (pos_mov_l1[c][h].ttypem[0] == 'B' && pos_mov_l1[c][h].ttypem[1] != 'A') || (pos_mov_l1[c][h].ttypem[0] == 'C') || (pos_mov_l1[c][h].ttypem[0] == 'D'))
                    {
                        harvested_or_not = check_if_protein_is_harvested(pos_mov_l1[c][h], my_list, entity_count);
                        if (pos_mov_l1[c][h].ttypem[0] == 'A' && harvester_a != 1)
                            harvested_or_not += 1;
                        if (pos_mov_l1[c][h].ttypem[0] == 'B' && harvester_b != 1)
                            harvested_or_not += 1;
                        if (pos_mov_l1[c][h].ttypem[0] == 'C' && harvester_c != 1)
                            harvested_or_not += 1;
                        if (pos_mov_l1[c][h].ttypem[0] == 'D' && harvester_d != 1)
                            harvested_or_not += 1;
                    }
                    if (harvested_or_not == 0)
                    {
                        if (my_a > 0)
                            printf("GROW %d %d %d BASIC N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                        else if (my_b > 0 && my_c > 0)
                            printf("GROW %d %d %d TENTACLE N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                        else if (my_b > 0 && my_d > 0)
                            printf("GROW %d %d %d SPORER N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                        else if (my_d > 0 && my_c > 0)
                            printf("GROW %d %d %d HARVESTER N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                        else
                            printf("GROW %d %d %d BASIC N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                        move_made += 1;
                    }
                }
                h++;
            }
            c++;
        }
    }
    i_c_tmp = i_oc;
    c = 0;
    if (move_made == 0)
    {
        while(c <= i_c_tmp && move_made == 0)
        {
            h = 0;
            while (h < 4 && move_made == 0)
            {
                harvested_or_not = 0;
                if (pos_mov_l1[c][h].validity == 1)
                {
                    if((pos_mov_l1[c][h].ttypem[0] == 'A') || (pos_mov_l1[c][h].ttypem[0] == 'B' && pos_mov_l1[c][h].ttypem[1] != 'A') || (pos_mov_l1[c][h].ttypem[0] == 'C') || (pos_mov_l1[c][h].ttypem[0] == 'D'))
                        harvested_or_not = check_if_protein_is_harvested(pos_mov_l1[c][h], my_list, entity_count);
                    if (harvested_or_not == 0)
                    {
                        if (my_a > 0)
                            printf("GROW %d %d %d BASIC N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                        else if (my_b > 0 && my_c > 0)
                            printf("GROW %d %d %d TENTACLE N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                        else if (my_b > 0 && my_d > 0)
                            printf("GROW %d %d %d SPORER N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                        else if (my_d > 0 && my_c > 0)
                            printf("GROW %d %d %d HARVESTER N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                        else
                            printf("GROW %d %d %d BASIC N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                        move_made += 1;
                    }
                }
                h++;
            }
            c++;
        }
    }
    i_c_tmp = i_oc;
    c = 0;
    if (move_made == 0)
    {
        while(c <= i_c_tmp && move_made == 0)
        {
            h = 0;
            while (h < 4 && move_made == 0)
            {
                if (pos_mov_l1[c][h].validity == 1)
                {
                    if (my_a > 0)
                        printf("GROW %d %d %d BASIC N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                    else if (my_b > 0 && my_c > 0)
                        printf("GROW %d %d %d TENTACLE N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                    else if (my_b > 0 && my_d > 0)
                        printf("GROW %d %d %d SPORER N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                    else if (my_d > 0 && my_c > 0)
                        printf("GROW %d %d %d HARVESTER N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                    else
                        printf("GROW %d %d %d BASIC N\n", organ_list[c].myorganid, pos_mov_l1[c][h].xxm, pos_mov_l1[c][h].yym);
                    move_made += 1;
                }
                h++;
            }
            c++;
        }
    }
    if (move_made == 0)
    {
        printf("GROW %d 1 1 BASIC N\n", organ_list[0].organrootid);
        move_made += 1;
    }

    i_c_tmp = i_oc;
    while (i_c_tmp >= 0)
    {
        free_counter_l1 = 0;
        free_counter_l2 = 0;
        while (free_counter_l2 < 8)
        {
            free(pos_mov_l2[i_c_tmp][free_counter_l2].move_loc);
            free_counter_l2 += 1;
        }
        while (free_counter_l1 < 4)
        {
            free(pos_mov_l1[i_c_tmp][free_counter_l1].move_loc);
            free_counter_l1 += 1;
        }
        free(pos_mov_l1[i_c_tmp]);
        free(pos_mov_l2[i_c_tmp]);
        i_c_tmp -= 1;
    }
    free(organ_list);
}

int check_if_protein_is_harvested(move_id_list protein, id_list *my_list, int entity_count)
{
    int count_tmp = entity_count - 1;
    while(count_tmp >= 0)
    {
        if(my_list[count_tmp].ownerid == 1 && my_list[count_tmp].ttype[0] == 'H')
        {
            if ((my_list[count_tmp].organdirr[0] == 'N') && (protein.xxm == my_list[count_tmp].xx) && (protein.yym == my_list[count_tmp].yy - 1))
                return (1);
            else if ((my_list[count_tmp].organdirr[0] == 'S') && (protein.xxm == my_list[count_tmp].xx) && (protein.yym == my_list[count_tmp].yy + 1))
                return (1);
            else if ((my_list[count_tmp].organdirr[0] == 'E') && (protein.xxm == my_list[count_tmp].xx + 1) && (protein.yym == my_list[count_tmp].yy))
                return (1);
            else if ((my_list[count_tmp].organdirr[0] == 'W') && (protein.xxm == my_list[count_tmp].xx - 1) && (protein.yym == my_list[count_tmp].yy))
                return (1);
        }
        count_tmp -=1;
    }
    return (0);
}
