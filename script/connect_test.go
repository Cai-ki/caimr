package script

import (
	"fmt"
	"log"
	"net"
	"sync"
	"testing"
	"time"
)

func TestEasyConnect(t *testing.T) {
	conn, err := net.Dial("tcp", "localhost:9999")
	if err != nil {
		log.Fatal(err)
	}

	read := sync.OnceFunc(func() {
		go func() {
			for {
				buffer := make([]byte, 1024)
				n, err := conn.Read(buffer)
				log.Println("receive", n, err, string(buffer[:n]))
			}
		}()
	})
	cnt := 0
	for {
		cnt++
		n, err := conn.Write([]byte("hello, world!" + fmt.Sprintf(" %d", cnt)))
		log.Println("send:", n, err)
		read()
		time.Sleep(1 * time.Millisecond)
	}
}
