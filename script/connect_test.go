package script

import (
	"bytes"
	"fmt"
	"io"
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
				buf := make([]byte, 1024)
				n, err := conn.Read(buf)
				log.Println("receive", n, err, string(buf[:n]))
			}
		}()
	})
	cnt := 0
	for {
		cnt++
		n, err := conn.Write([]byte("hello, world!" + fmt.Sprintf(" %d", cnt)))
		log.Println("send:", n, err)
		read()
		time.Sleep(1 * time.Second)
	}
}

func BenchmarkEasyConnect(b *testing.B) {
	conn, err := net.Dial("tcp", "localhost:9999")
	if err != nil {
		b.Fatal(err)
	}

	read := sync.OnceFunc(func() {
		go func() {
			buf := make([]byte, len("hello, world!"))
			for {
				n, err := io.ReadFull(conn, buf)
				if err != nil {
					b.Error(err)
				}
				if string(buf[:n]) != "hello, world!" {
					b.Error("receive", n, err, string(buf[:n]))
				}
			}
		}()
	})

	for i := 0; i < b.N; i++ {
		n, err := conn.Write([]byte("hello, world!"))
		if err != nil {
			b.Error("send:", n, err)
		}
		read()
	}
}

func BenchmarkManyConnect(b *testing.B) {
	connCnt := 1000
	queryPerConn := max(b.N/connCnt, 1)
	bytesLen := 1024
	bytesBuf := make([]byte, bytesLen)
	for i := range bytesBuf {
		bytesBuf[i] = byte(i % 10)
	}

	msg := "hello, world!"
	bytesLen = len(msg)
	bytesBuf = []byte(msg)

	b.Logf("%d conn and %d query per conn, each msg %d byte", connCnt, queryPerConn, bytesLen)

	wgConn := sync.WaitGroup{}
	wgConn.Add(connCnt)

	b.ResetTimer()
	for _ = range connCnt {
		go func() {
			conn, err := net.Dial("tcp", "localhost:9999")

			if err != nil {
				b.Error(err)
				return
			}

			read := sync.OnceFunc(func() {
				go func() {
					defer func() {
						conn.Close()
						wgConn.Done()
					}()

					buf := make([]byte, bytesLen)
					for _ = range queryPerConn {
						n, err := io.ReadFull(conn, buf)
						if err != nil {
							b.Error(err)
						}
						if !bytes.Equal(buf[:n], bytesBuf) {
							b.Error("receive", n, err, buf[:n], bytesBuf)
						}
					}
				}()
			})

			for _ = range queryPerConn {
				n, err := conn.Write(bytesBuf)
				if err != nil {
					b.Error("send:", n, err)
				}
				read()
			}
		}()
	}

	wgConn.Wait()
}
