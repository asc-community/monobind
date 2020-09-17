using System;
using System.Runtime.CompilerServices;

namespace MonoBindExamples
{
    class Player
    {
        string _name;
        int _health;

        public Player(string name, int health)
        {
            _name = name;
            _health = health;
        }

        public override string ToString()
        {
            return $"Player {{ name={_name}, health={_health} }}";
        }
    }

    class Monster
    {
        string _name;
        int _health;
        int _damage;

        public Monster(string name, int health, int damage)
        {
            _name = name;
            _health = health;
            _damage = damage;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void Attack(Player player);
    }
}