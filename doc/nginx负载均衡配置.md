# nginx负载均衡配置

---

```nginx
# nginx tcp loadbalance configuration
stream {
    upstream ChatServer {
        server 192.168.172.134:20000 weight=1 max_fails=3 fail_timeout=30s;
        server 192.168.172.137:20000 weight=1 max_fails=3 fail_timeout=30s;
        server 192.168.172.138:20000 weight=1 max_fails=3 fail_timeout=30s;
    }
    
    server {
        proxy_connect_timeout 1s;
        #proxy_timeout 3s;
        listen 8001;
        proxy_pass ChatServer;
        tcp_nodelay on;
    }
}
```

