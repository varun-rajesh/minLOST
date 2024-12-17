import utils

def binary_search(arr, target):
    left, right = 0, len(arr) - 1
    closest_index = -1
    
    while left <= right:
        mid = (left + right) // 2
        if arr[mid] < target:
            closest_index = mid
            left = mid + 1
        else:
            right = mid - 1
            
    return closest_index


def _get_candidates(stars_a, stars_b, angles, target_angle):
    candidates = dict()
    
    start_index = binary_search(angles, target_angle - 0.0001)
    
    for i in range(start_index, len(angles)):
        if angles[i] < target_angle + 0.0001:
            if stars_a[i] not in candidates:
                candidates[stars_a[i]] = set()
            if stars_b[i] not in candidates:
                candidates[stars_b[i]] = set()
            candidates[stars_a[i]].add(stars_b[i])
            candidates[stars_b[i]].add(stars_a[i])
        else:
            break
            
    return candidates
    

def _find_ijkr(stars_a, stars_b, angles, observed_angles):
    ij_angle = observed_angles[0]
    ij_candidates = _get_candidates(stars_a, stars_b, angles, ij_angle)
    
    ik_angle = observed_angles[1]
    ik_candidates = _get_candidates(stars_a, stars_b, angles, ik_angle)
    
    jk_angle = observed_angles[3]
    jk_candidates = _get_candidates(stars_a, stars_b, angles, jk_angle)
    
    found_i = []
    found_j = []
    found_k = []
    
    for i_candidate in ij_candidates:
        j_candidates = ij_candidates[i_candidate]
        
        for j_candidate in j_candidates:
            if j_candidate in jk_candidates:
                k_candidates = jk_candidates[j_candidate]
        
                for k_candidate in k_candidates:
                    if k_candidate in ik_candidates:
                        if i_candidate in ik_candidates[k_candidate]:
                            found_i.append(i_candidate)
                            found_j.append(j_candidate)
                            found_k.append(k_candidate)
                            
    ir_angle = observed_angles[2]
    jr_angle = observed_angles[4]
       
    ir_candidates = _get_candidates(stars_a, stars_b, angles, ir_angle)
    jr_candidates = _get_candidates(stars_a, stars_b, angles, jr_angle)
    
    candidate_found = False 
    
    i, j, k, r = None, None, None, None
    
    for i_candidate, j_candidate, k_candidate in zip(found_i, found_j, found_k):
        if i_candidate in ir_candidates:
            r_candidates = ir_candidates[i_candidate]
            
            for r_candidate in r_candidates:
                if r_candidate in jr_candidates:
                    j_candidates = jr_candidates[r_candidate]

                    if j_candidate in j_candidates:
                        if candidate_found:
                            return None
                        else:
                            candidate_found = True
                            
                        i, j, k, r = i_candidate, j_candidate, k_candidate, r_candidate
    
    return i, j, k, r


def star_identifier(database, star_centers):
    stars_a, stars_b, angles = database
    
    for start in range(len(star_centers) - 4):
        
        observed_angles = []
        for i in range(start, start + 4):
            for j in range(i + 1, start + 4):
                print(i, j)
                observed_angles.append(utils.angle(star_centers[i], star_centers[j]))
        
        print(observed_angles)
        ijkr = _find_ijkr(stars_a, stars_b, angles, observed_angles)
        if ijkr is None:
            continue
        else:
            print(ijkr)
            extra_count = 0
            for extra_star in star_centers[start + 4:]:
                e = _identify_extra_stars(stars_a, stars_b, angles, ijkr, star_centers[start:start + 4], extra_star)
                if e:
                    extra_count += 1
                    print(e, extra_count)
            
            return ijkr
    

def _identify_extra_stars(stars_a, stars_b, angles, ijkr, star_centers_ijkr, extra_star):
    i, j, k, r = ijkr
    
    ie_angle = utils.angle(star_centers_ijkr[0], extra_star)
    ie_candidates = _get_candidates(stars_a, stars_b, angles, ie_angle)
    
    je_angle = utils.angle(star_centers_ijkr[1], extra_star)
    je_candidates = _get_candidates(stars_a, stars_b, angles, je_angle)
    
    if i in ie_candidates:
        e_candidates = ie_candidates[i]
        
        for e_candidate in e_candidates:
            if e_candidate in je_candidates:
                if j in je_candidates[e_candidate]:
                    return e_candidate
                    