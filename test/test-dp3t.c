/* The Fastest Proximity Tracing in the West (FPTW)
 * aka the Secret Pangolin Code
 *
 * Copyright (C) 2020 Dyne.org foundation
 * designed, written and maintained by Daniele Lacamera and Denis Roio
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <inttypes.h>
#include <assert.h>
#include <string.h>

#include <dp3t.h>

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha256.h>

#include <bdd-for-c.h>
#include <aux.h>
#include <vectors.h>
#include <positives_20k.h>

#define TTL 15

void print_beacons(uint8_t *sk) {
	beacons_t *beacons;
	beacons = gcalloc(sizeof(beacons_t)+(100*sizeof(beacon_t)));
	assert(0==generate_beacons(beacons, 100, sk, 0, TTL, hex(BK), BKLEN));
	register int i;
	for(i=0;i<beacons->epochs;i++)
		fprintf(stderr,"%u %s\n",i,bin16(beacons->ephids[i]));
}

spec("DP-3T") {

	before() {
		printf("version: %s\n",VERSION);
		printf("platform: %s\n", PLATFORM);
	}
	after() {
		// 
	}
	describe("lowcost") {
		static sk_t sk;
		static contacts_t positives;

		before() {
			gcinit(&head);
			memset(sk,0x0,32);
		}
		after_each() {
			//
		}
		after() {
			// free_beacons((beacons_t*)b);
			gcfree(&head); // free all strings
		}

		it("should use 'Broadcast key'") {
			check( compare(hex(BK), "Broadcast key", BKLEN) );
			wc_Sha256 sha;
			uint8_t dest[32];
			assert( wc_InitSha256(&sha) == 0);
			wc_Sha256Update(&sha, hex(BK), BKLEN);
			wc_Sha256Final(&sha, dest);
			wc_Sha256Free(&sha);
			check( compare(hex(BKHASH), dest, 32) );
		}
		it("should have zero derived SKs matching test vectors...") {
			renew_key(sk, hex(ZERO32));
			check( compare(sk, hex(SK1), 32) );
			renew_key(sk, sk);
			check( compare(sk, hex(SK2), 32) );
		}
		it("should have zero derived EphIDs matching test vectors...") {
			static beacons_t *beacons;
			beacons = gcalloc(sizeof(beacons_t)+(100*sizeof(beacon_t)));
			check(0== generate_beacons(beacons, 100, hex(ZERO32), 0, TTL, hex(BK), BKLEN) );
			// B16(beacons->ephids[0]);
			check( compare(beacons->ephids[0], hex(ephid0), 16),
			       "Invalid ephid 0:\n"
			       "%s != %s",bin16(beacons->ephids[0]), ephid0 );
			check( compare(beacons->ephids[8], hex(ephid8), 16),
			       "Invalid ephid 8:\n"
			       "%s != %s",bin16(beacons->ephids[7]), ephid8 );
		}

		it("should have a needle vector among 20.000 matching 5 beacons...") {
			static matches_t *matches;
			matches = gcalloc(sizeof(matches_t)+(10*sizeof(contact_t*)));
			static contacts_t contacts;
			memcpy(contacts.broadcast, hex(BK), BKLEN);
			contacts.broadcast_len = BKLEN;
			contacts.epochs = ((24*60) / TTL) +1;
			contacts.ephids = test_ephids;
			contacts.count  = 14;
			static uint8_t positives[20000<<5];
			hex2binlen(positives, positive_sks, 20000<<5);
			register int i;
			static uint32_t ret;
			for(i=0;i<20000;i++) {
				ret += match_positive(matches, 10, &positives[i<<5], &contacts);
			}
			check( ret == 5, "Wrong matches: %u (should be 5)", ret );
		}
	}
}
