
extern void ipt_ah_init(void);
extern void ipt_addrtype_init(void);
extern void ipt_conntrack_init(void);
extern void ipt_ecn_init(void);
extern void ipt_icmp_init(void);
extern void ipt_iprange_init(void);
extern void ipt_owner_init(void);
extern void ipt_policy_init(void);
extern void ipt_realm_init(void);
extern void ipt_recent_init(void);
extern void ipt_tos_init(void);
extern void ipt_ttl_init(void);
extern void ipt_unclean_init(void);
extern void ipt_IMQ_init(void);
extern void ipt_CLUSTERIP_init(void);
extern void ipt_DNAT_init(void);
extern void ipt_ECN_init(void);
extern void ipt_LOG_init(void);
extern void ipt_MASQUERADE_init(void);
extern void ipt_MIRROR_init(void);
extern void ipt_NETMAP_init(void);
extern void ipt_REDIRECT_init(void);
extern void ipt_REJECT_init(void);
extern void ipt_ROUTE_init(void);
extern void ipt_SAME_init(void);
extern void ipt_SNAT_init(void);
extern void ipt_TOS_init(void);
extern void ipt_TTL_init(void);
extern void ipt_ULOG_init(void);
extern void ipt_layer7_init(void);
extern void ipt_webstr_init(void);
extern void xt_connbytes_init(void);
extern void xt_connmark_init(void);
extern void xt_connlimit_init(void);
extern void xt_comment_init(void);
extern void xt_dccp_init(void);
extern void xt_dscp_init(void);
extern void xt_esp_init(void);
extern void xt_ethport_init(void);
extern void xt_hashlimit_init(void);
extern void xt_helper_init(void);
extern void xt_length_init(void);
extern void xt_limit_init(void);
extern void xt_mac_init(void);
extern void xt_mark_init(void);
extern void xt_multiport_init(void);
extern void xt_physdev_init(void);
extern void xt_pkttype_init(void);
extern void xt_quota_init(void);
extern void xt_sctp_init(void);
extern void xt_state_init(void);
extern void xt_statistic_init(void);
extern void xt_standard_init(void);
extern void xt_string_init(void);
extern void xt_tcp_init(void);
extern void xt_tcpmss_init(void);
extern void xt_time_init(void);
extern void xt_u32_init(void);
extern void xt_udp_init(void);
extern void xt_CLASSIFY_init(void);
extern void xt_CONNMARK_init(void);
extern void xt_DSCP_init(void);
extern void xt_MARK_init(void);
extern void xt_NFLOG_init(void);
extern void xt_NFQUEUE_init(void);
extern void xt_NOTRACK_init(void);
extern void xt_TCPMSS_init(void);
extern void xt_TRACE_init(void);
void init_extensions(void) {
	ipt_ah_init();
	ipt_addrtype_init();
	ipt_conntrack_init();
	ipt_ecn_init();
	ipt_icmp_init();
	ipt_iprange_init();
	ipt_owner_init();
	ipt_policy_init();
	ipt_realm_init();
	ipt_recent_init();
	ipt_tos_init();
	ipt_ttl_init();
	ipt_unclean_init();
	ipt_IMQ_init();
	ipt_CLUSTERIP_init();
	ipt_DNAT_init();
	ipt_ECN_init();
	ipt_LOG_init();
	ipt_MASQUERADE_init();
	ipt_MIRROR_init();
	ipt_NETMAP_init();
	ipt_REDIRECT_init();
	ipt_REJECT_init();
	ipt_ROUTE_init();
	ipt_SAME_init();
	ipt_SNAT_init();
	ipt_TOS_init();
	ipt_TTL_init();
	ipt_ULOG_init();
	ipt_layer7_init();
	ipt_webstr_init();
	xt_connbytes_init();
	xt_connmark_init();
	xt_connlimit_init();
	xt_comment_init();
	xt_dccp_init();
	xt_dscp_init();
	xt_esp_init();
	xt_ethport_init();
	xt_hashlimit_init();
	xt_helper_init();
	xt_length_init();
	xt_limit_init();
	xt_mac_init();
	xt_mark_init();
	xt_multiport_init();
	xt_physdev_init();
	xt_pkttype_init();
	xt_quota_init();
	xt_sctp_init();
	xt_state_init();
	xt_statistic_init();
	xt_standard_init();
	xt_string_init();
	xt_tcp_init();
	xt_tcpmss_init();
	xt_time_init();
	xt_u32_init();
	xt_udp_init();
	xt_CLASSIFY_init();
	xt_CONNMARK_init();
	xt_DSCP_init();
	xt_MARK_init();
	xt_NFLOG_init();
	xt_NFQUEUE_init();
	xt_NOTRACK_init();
	xt_TCPMSS_init();
	xt_TRACE_init();
}