# Configurações RCOM Network Lab

(até ao início da experiência 3)


> ⚠ - **Guardar configurações Switch/Router:**
> 
> `#> copy running-config flash:<nomedoficheiro>` nomedoficheiro → **config_t7g1_leic**
>
> **Reposição das configurações**:
> 
> 1. Reposição das configs de acordo com o indicado na bancada
> 2. `#>copy flash:<nomedoficheiro> running-config`

## Configurações Exp.1 

```bash
# PC 3
ifconfig eth0 172.16.20.1/24
route -n # verificar se tem 172.16.20.0 (mac 00:21:5a:5a:78:c7)

# PC 4
ifconfig eth0 172.16.20.254/24
route -n # verificar se tem 172.16.20.0 (mac 00:22:64:a7:26:a2)
# verificar ping (ping 172.16.20.1)
# verificar arp -a se o mac adress no eth0 é igual
```

## Configurações Exp. 2


`NºPorta = nºPC * 2 + ethN`   
         - (**N => nº da rede**)

```bash
# PC 2
if config eth0 172.16.21.1/24 # (mac 00:21:5a:5a:76:a8)

# No PC ligado ao S0 - GTK Term
# Configurar VLAN0 e VLAN1
enable
## password 8nortel

#VLAN 0
configure terminal
vlan 20
end

#Adicionar port 6 ao VLAN 20
configure terminal # entrar no modo de config
interface fastethernet 0/6
switchport mode access
switchport access vlan 20
end

# para verificar
show running-config interface fastethernet 0/6
show interfaces fastethernet 0/6 switchport

#Adicionar port 8 ao VLAN 20
configure terminal # entrar no modo de config
interface fastethernet 0/8
switchport mode access
switchport access vlan 20
end

# para verificar
show running-config interface fastethernet 0/8
show interfaces fastethernet 0/8 switchport

#VLAN 1
configure terminal
vlan 21
end

#Adicionar port 4 ao VLAN 21
configure terminal # entrar no modo de config
interface fastethernet 0/4
switchport mode access
switchport access vlan 21
end

# para verificar
show running-config interface fastethernet 0/4
show interfaces fastethernet 0/4 switchport

#Para verificar se tudo certo no final
show vlan brief
```

## Configurações Exp. 3/4

```bash
# No PC que est+a ligado ao S0 no GTK term para configurar o router

#Adicionar port 9 ao VLAN 21
configure terminal # entrar no modo de config
interface fastethernet 0/9
switchport mode access
switchport access vlan 21
end

# para verificar
show vlan brief

# PC 4
#Configurar IP address
ifconfig eth1 172.16.21.253/24

# Enable IP forwarding
echo 1 > /proc/sys/net/ipv4/ip_forward

# Disable ICMP echo ignore broadcast.
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

# PC 3 
ip route add 172.16.21.0/24 via 172.16.20.254

# PC 2
ip route add 172.16.20.0/24 via 172.16.21.253

```
